# Project 4 Report

## 1. Proof of Your Knowledge

### 1.1

#### 1.1.1

Check the code in `src/assembly/scenario_select.S`

#### 1.1.2

`int scenario_select(int initial_selection)` takes an integer as an argument and returns an integer.

#### 1.1.3

`int scenario_select(int initial_selection)` modifies the global variable `scene_number` to pass the selected scenario number to the main function.

#### 1.1.4

dis-assembly of `Board_self_test` is shown below:

The loop `while(1)` is used and the corresponding code is `j 80011ce`.

Some instructions are shown in 4 hex digits form, they are compressed instructions which take up 2 bytes.

```riscv
0800119a <Board_self_test>:
 800119a:	7179                	addi	sp,sp,-48
 800119c:	d422                	sw	s0,40(sp)
 800119e:	d226                	sw	s1,36(sp)
 80011a0:	d04a                	sw	s2,32(sp)
 80011a2:	ce4e                	sw	s3,28(sp)
 80011a4:	cc52                	sw	s4,24(sp)
 80011a6:	ca56                	sw	s5,20(sp)
 80011a8:	c85a                	sw	s6,16(sp)
 80011aa:	c65e                	sw	s7,12(sp)
 80011ac:	d606                	sw	ra,44(sp)
 80011ae:	08001437          	lui	s0,0x8001
 80011b2:	080014b7          	lui	s1,0x8001
 80011b6:	08001937          	lui	s2,0x8001
 80011ba:	080019b7          	lui	s3,0x8001
 80011be:	08001a37          	lui	s4,0x8001
 80011c2:	08001ab7          	lui	s5,0x8001
 80011c6:	08001b37          	lui	s6,0x8001
 80011ca:	08001bb7          	lui	s7,0x8001
 80011ce:	66c1                	lui	a3,0x10
 80011d0:	03c00513          	li	a0,60
 80011d4:	16fd                	addi	a3,a3,-1
 80011d6:	85440613          	addi	a2,s0,-1964 # 8000854 <asc2_1608+0x5f0>
 80011da:	45e5                	li	a1,25
 80011dc:	3f11                	jal	80010f0 <LCD_ShowString>
 80011de:	4501                	li	a0,0
 80011e0:	22a1                	jal	8001328 <Get_Button>
 80011e2:	c519                	beqz	a0,80011f0 <Board_self_test+0x56>
 80011e4:	46fd                	li	a3,31
 80011e6:	86048613          	addi	a2,s1,-1952 # 8000860 <asc2_1608+0x5fc>
 80011ea:	45e5                	li	a1,25
 80011ec:	4515                	li	a0,5
 80011ee:	3709                	jal	80010f0 <LCD_ShowString>
 80011f0:	4505                	li	a0,1
 80011f2:	2a1d                	jal	8001328 <Get_Button>
 80011f4:	c105                	beqz	a0,8001214 <Board_self_test+0x7a>
 80011f6:	46fd                	li	a3,31
 80011f8:	86490613          	addi	a2,s2,-1948 # 8000864 <asc2_1608+0x600>
 80011fc:	02d00593          	li	a1,45
 8001200:	4565                	li	a0,25
 8001202:	35fd                	jal	80010f0 <LCD_ShowString>
 8001204:	7e000693          	li	a3,2016
 8001208:	86898613          	addi	a2,s3,-1944 # 8000868 <asc2_1608+0x604>
 800120c:	45e5                	li	a1,25
 800120e:	03c00513          	li	a0,60
 8001212:	3df9                	jal	80010f0 <LCD_ShowString>
 8001214:	450d                	li	a0,3
 8001216:	2a09                	jal	8001328 <Get_Button>
 8001218:	c519                	beqz	a0,8001226 <Board_self_test+0x8c>
 800121a:	46fd                	li	a3,31
 800121c:	870a0613          	addi	a2,s4,-1936 # 8000870 <asc2_1608+0x60c>
 8001220:	4595                	li	a1,5
 8001222:	4565                	li	a0,25
 8001224:	35f1                	jal	80010f0 <LCD_ShowString>
 8001226:	4509                	li	a0,2
 8001228:	2201                	jal	8001328 <Get_Button>
 800122a:	c901                	beqz	a0,800123a <Board_self_test+0xa0>
 800122c:	46fd                	li	a3,31
 800122e:	874a8613          	addi	a2,s5,-1932 # 8000874 <asc2_1608+0x610>
 8001232:	45e5                	li	a1,25
 8001234:	02d00513          	li	a0,45
 8001238:	3d65                	jal	80010f0 <LCD_ShowString>
 800123a:	4511                	li	a0,4
 800123c:	20f5                	jal	8001328 <Get_Button>
 800123e:	c519                	beqz	a0,800124c <Board_self_test+0xb2>
 8001240:	46fd                	li	a3,31
 8001242:	878b0613          	addi	a2,s6,-1928 # 8000878 <asc2_1608+0x614>
 8001246:	45e5                	li	a1,25
 8001248:	4565                	li	a0,25
 800124a:	355d                	jal	80010f0 <LCD_ShowString>
 800124c:	4515                	li	a0,5
 800124e:	28e9                	jal	8001328 <Get_Button>
 8001250:	c901                	beqz	a0,8001260 <Board_self_test+0xc6>
 8001252:	46fd                	li	a3,31
 8001254:	87cb8613          	addi	a2,s7,-1924 # 800087c <asc2_1608+0x618>
 8001258:	4595                	li	a1,5
 800125a:	03c00513          	li	a0,60
 800125e:	3d49                	jal	80010f0 <LCD_ShowString>
 8001260:	4519                	li	a0,6
 8001262:	20d9                	jal	8001328 <Get_Button>
 8001264:	c919                	beqz	a0,800127a <Board_self_test+0xe0>
 8001266:	08001637          	lui	a2,0x8001
 800126a:	46fd                	li	a3,31
 800126c:	88060613          	addi	a2,a2,-1920 # 8000880 <asc2_1608+0x61c>
 8001270:	02d00593          	li	a1,45
 8001274:	03c00513          	li	a0,60
 8001278:	3da5                	jal	80010f0 <LCD_ShowString>
 800127a:	4529                	li	a0,10
 800127c:	2021                	jal	8001284 <delay_1ms>
 800127e:	4501                	li	a0,0
 8001280:	3b91                	jal	8000fd4 <LCD_Clear>
 8001282:	b7b1                	j	80011ce <Board_self_test+0x34>
```
