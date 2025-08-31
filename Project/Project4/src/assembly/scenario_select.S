.section .data
.global scene_number
scene_number:
    .word 1
.section .text
.global scenario_select

scenario_select:
    # Save return address and t0
    addi sp, sp, -8
    sw ra, 0(sp)
    sw t0, 4(sp)

    # Initialize selection to the first parameter
    mv t0, a0

    # Clear screen
    li a0, 0  # BLACK
    call LCD_Clear

    call display_scenarios

selection_loop:
    # Save t0 before calling Get_Button
    sw t0, 4(sp)
    # Check for UP button (JOY_UP = 3)
    li a0, 3
    call Get_Button
    # Restore t0 after Get_Button
    lw t0, 4(sp)
    beqz a0, check_down
    # If UP pressed, move selection up
    addi t0, t0, -1
    # Ensure selection is at least 1
    li t1, 1
    blt t0, t1, set_to_one
    j update_display
set_to_one:
    li t0, 1
update_display:
    # Save t0 before calling display_scenarios
    sw t0, 4(sp)
    call display_scenarios
    # Restore t0 after display_scenarios
    lw t0, 4(sp)
    # Save t0 before calling delay_1ms
    sw t0, 4(sp)
    li a0, 400
    call delay_1ms
    # Restore t0 after delay_1ms
    lw t0, 4(sp)

check_down:
    # Save t0 before calling Get_Button
    sw t0, 4(sp)
    # Check for DOWN button (JOY_DOWN = 1)
    li a0, 1
    call Get_Button
    # Restore t0 after Get_Button
    lw t0, 4(sp)
    beqz a0, check_center
    # If DOWN pressed, move selection down
    addi t0, t0, 1
    # Ensure selection is at most 3
    li t1, 3
    bgt t0, t1, set_to_three
    j update_display2
set_to_three:
    li t0, 3
update_display2:
    # Save t0 before calling display_scenarios
    sw t0, 4(sp)
    call display_scenarios
    # Restore t0 after display_scenarios
    lw t0, 4(sp)
    # Save t0 before calling delay_1ms
    sw t0, 4(sp)
    li a0, 400
    call delay_1ms
    # Restore t0 after delay_1ms
    lw t0, 4(sp)

check_center:
    # Save t0 before calling Get_Button
    sw t0, 4(sp)
    # Check for CENTER button (JOY_CTR = 4)
    li a0, 4
    call Get_Button
    # Restore t0 after Get_Button
    lw t0, 4(sp)
    beqz a0, selection_loop
    # If CENTER pressed, store selection in global variable and return 0
    la t1, scene_number
    sw t0, 0(t1)
    li a0, 0  # Return 0 for success
    # Restore return address and t0
    lw ra, 0(sp)
    lw t0, 4(sp)
    addi sp, sp, 8
    ret

# Display all scenarios with selection marker
display_scenarios:
    # Save return address and t0
    addi sp, sp, -8
    sw ra, 0(sp)
    sw t0, 4(sp)

    # Clear the marker
    li a0, 5
    li a1, 5
    la a2, marker_str
    li a3, 0 # BLACK
    call LCD_ShowString

    li a0, 5
    li a1, 20
    la a2, marker_str
    li a3, 0 # BLACK
    call LCD_ShowString

    li a0, 5
    li a1, 35
    la a2, marker_str
    li a3, 0 # BLACK
    call LCD_ShowString

    # Display "Scenario 1"
    li a0, 20
    li a1, 5
    la a2, scenario1_str
    li a3, 0xFFFF  # WHITE
    call LCD_ShowString

    # Display "Scenario 2"
    li a0, 20
    li a1, 20
    la a2, scenario2_str
    li a3, 0xFFFF  # WHITE
    call LCD_ShowString

    # Display "Scenario 3"
    li a0, 20
    li a1, 35
    la a2, scenario3_str
    li a3, 0xFFFF  # WHITE
    call LCD_ShowString

    # Display selection marker based on t0
    li t1, 1
    beq t0, t1, show_marker1
    li t1, 2
    beq t0, t1, show_marker2
    li t1, 3
    beq t0, t1, show_marker3
    j display_done

show_marker1:
    li a0, 5
    li a1, 5
    la a2, marker_str
    li a3, 0x07E0  # GREEN
    call LCD_ShowString
    j display_done

show_marker2:
    li a0, 5
    li a1, 20
    la a2, marker_str
    li a3, 0x07E0  # GREEN
    call LCD_ShowString
    j display_done

show_marker3:
    li a0, 5
    li a1, 35
    la a2, marker_str
    li a3, 0x07E0  # GREEN
    call LCD_ShowString

display_done:
    # Restore return address and t0
    lw ra, 0(sp)
    lw t0, 4(sp)
    addi sp, sp, 8
    ret

.section .rodata
scenario1_str: .string "Scenario 1"
scenario2_str: .string "Scenario 2"
scenario3_str: .string "Scenario 3"
marker_str: .string ">" 