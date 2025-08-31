use rayon::prelude::*;

const CHACHA_BLOCK_SIZE: usize = 64;
const CHACHA_ROUNDS: usize = 10;

#[inline(always)]
fn rotl32(x: u32, n: u32) -> u32 {
    x.rotate_left(n)
}

#[inline(always)]
fn quarter_round(x: &mut [u32; 16], a: usize, b: usize, c: usize, d: usize) {
    x[a] = x[a].wrapping_add(x[b]);
    x[d] = rotl32(x[d] ^ x[a], 16);
    x[c] = x[c].wrapping_add(x[d]);
    x[b] = rotl32(x[b] ^ x[c], 12);
    x[a] = x[a].wrapping_add(x[b]);
    x[d] = rotl32(x[d] ^ x[a], 8);
    x[c] = x[c].wrapping_add(x[d]);
    x[b] = rotl32(x[b] ^ x[c], 7);
}

#[inline(always)]
fn chacha20_block(state: &[u32; 16], out: &mut [u8; 64]) {
    let mut working_state = *state;

    for _ in 0..CHACHA_ROUNDS {
        // Column rounds
        quarter_round(&mut working_state, 0, 4, 8, 12);
        quarter_round(&mut working_state, 1, 5, 9, 13);
        quarter_round(&mut working_state, 2, 6, 10, 14);
        quarter_round(&mut working_state, 3, 7, 11, 15);

        // Diagonal rounds
        quarter_round(&mut working_state, 0, 5, 10, 15);
        quarter_round(&mut working_state, 1, 6, 11, 12);
        quarter_round(&mut working_state, 2, 7, 8, 13);
        quarter_round(&mut working_state, 3, 4, 9, 14);
    }

    // Add the original state
    for i in 0..16 {
        working_state[i] = working_state[i].wrapping_add(state[i]);
    }

    // Convert to little-endian bytes
    for i in 0..16 {
        let word = working_state[i];
        out[i * 4] = (word & 0xff) as u8;
        out[i * 4 + 1] = ((word >> 8) & 0xff) as u8;
        out[i * 4 + 2] = ((word >> 16) & 0xff) as u8;
        out[i * 4 + 3] = ((word >> 24) & 0xff) as u8;
    }
}

#[no_mangle]
pub extern "C" fn chacha20_encrypt(
    key: *const u8,
    nonce: *const u8,
    initial_counter: u32,
    buffer: *mut u8,
    length: usize,
) {
    let key = unsafe { std::slice::from_raw_parts(key, 32) };
    let nonce = unsafe { std::slice::from_raw_parts(nonce, 12) };
    let buffer = unsafe { std::slice::from_raw_parts_mut(buffer, length) };

    // Convert key and nonce to words
    let mut key_words = [0u32; 8];
    let mut nonce_words = [0u32; 3];

    for i in 0..8 {
        key_words[i] =
            u32::from_le_bytes([key[i * 4], key[i * 4 + 1], key[i * 4 + 2], key[i * 4 + 3]]);
    }

    for i in 0..3 {
        nonce_words[i] = u32::from_le_bytes([
            nonce[i * 4],
            nonce[i * 4 + 1],
            nonce[i * 4 + 2],
            nonce[i * 4 + 3],
        ]);
    }

    // Initialize state
    let state = [
        0x61707865,
        0x3320646e,
        0x79622d32,
        0x6b206574, // Constants
        key_words[0],
        key_words[1],
        key_words[2],
        key_words[3],
        key_words[4],
        key_words[5],
        key_words[6],
        key_words[7],
        initial_counter,
        nonce_words[0],
        nonce_words[1],
        nonce_words[2],
    ];

    // Process data in parallel chunks
    let chunk_size = 1024 * 1024; // 1MB chunks
    buffer
        .par_chunks_mut(chunk_size)
        .enumerate()
        .for_each(|(chunk_idx, chunk)| {
            let mut local_state = state;
            local_state[12] =
                state[12].wrapping_add((chunk_idx * chunk_size / CHACHA_BLOCK_SIZE) as u32);

            let mut key_stream = [0u8; CHACHA_BLOCK_SIZE];
            let mut offset = 0;

            while offset < chunk.len() {
                chacha20_block(&local_state, &mut key_stream);

                let block_size = std::cmp::min(CHACHA_BLOCK_SIZE, chunk.len() - offset);
                for i in 0..block_size {
                    chunk[offset + i] ^= key_stream[i];
                }

                offset += block_size;
                local_state[12] = local_state[12].wrapping_add(1);
            }
        });
}

#[no_mangle]
pub extern "C" fn merkel_tree(input: *const u8, output: *mut u8, length: usize) {
    let input = unsafe { std::slice::from_raw_parts(input, length) };
    let output = unsafe { std::slice::from_raw_parts_mut(output, 64) };

    // Allocate buffers for the tree computation
    let mut cur_buf = vec![0u8; length];
    let mut prev_buf = vec![0u8; length];
    prev_buf.copy_from_slice(input);

    let mut current_length = length / 2;
    while current_length >= 64 {
        // Process pairs of blocks in parallel
        let results: Vec<_> = (0..current_length / 64)
            .into_par_iter()
            .map(|i| {
                let block1 = &prev_buf[(2 * i) * 64..(2 * i + 1) * 64];
                let block2 = &prev_buf[(2 * i + 1) * 64..(2 * i + 2) * 64];

                // Convert blocks to words for processing
                let mut state = [0u32; 16];
                let w1: &[u32] =
                    unsafe { std::slice::from_raw_parts(block1.as_ptr() as *const u32, 16) };
                let w2: &[u32] =
                    unsafe { std::slice::from_raw_parts(block2.as_ptr() as *const u32, 16) };

                // Initial mixing
                for i in 0..8 {
                    state[i] = w1[i] ^ w2[7 - i];
                    state[8 + i] = w2[i] ^ w1[7 - i];
                }

                // Rounds
                for _ in 0..10 {
                    // First phase
                    for i in 0..4 {
                        state[i] = state[i].wrapping_add(state[4 + i]);
                        state[i] = rotl32(state[i], 7);
                        state[8 + i] = state[8 + i].wrapping_add(state[12 + i]);
                        state[8 + i] = rotl32(state[8 + i], 7);
                    }
                    // Second phase
                    for i in 0..4 {
                        state[i] = state[i].wrapping_add(state[8 + i]);
                        state[i] = rotl32(state[i], 9);
                        state[4 + i] = state[4 + i].wrapping_add(state[12 + i]);
                        state[4 + i] = rotl32(state[4 + i], 9);
                    }
                }

                // Final mixing
                for i in 0..8 {
                    state[i] = state[i].wrapping_add(state[15 - i]);
                }

                // Convert back to bytes
                let mut out = [0u8; 64];
                for i in 0..16 {
                    let word = state[i];
                    out[i * 4] = (word & 0xff) as u8;
                    out[i * 4 + 1] = ((word >> 8) & 0xff) as u8;
                    out[i * 4 + 2] = ((word >> 16) & 0xff) as u8;
                    out[i * 4 + 3] = ((word >> 24) & 0xff) as u8;
                }
                out
            })
            .collect();

        // Copy results back to cur_buf
        for (i, result) in results.into_iter().enumerate() {
            cur_buf[i * 64..(i + 1) * 64].copy_from_slice(&result);
        }

        current_length /= 2;
        std::mem::swap(&mut cur_buf, &mut prev_buf);
    }

    // Copy the final result
    output.copy_from_slice(&cur_buf[..64]);
}
