fn main() {
    println!("cargo:rustc-cfg=target_arch=\"x86_64\"");

    // Enable AVX2 features
    println!("cargo:rustc-cfg=target_feature=\"avx2\"");

    // Set optimization flags for native CPU features
    println!("cargo:rustc-env=RUSTFLAGS=-C target-cpu=native -C target-feature=+avx2");
}
