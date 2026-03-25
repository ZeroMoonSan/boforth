variable passed
variable failed

: assert-pass
  passed @ 1 + passed !
  " PASS" puts cr
;

: assert-fail
  failed @ 1 + failed !
  " FAIL" puts cr
;

: test ( name result -- )
  swap puts
  dup if assert-pass then
  not if assert-fail then
;

: results
  "Passed: " puts passed @ . cr
  "Failed: " puts failed @ . cr
;

\ арифметика
"2 + 3 = 5"    2 3 + 5 =    test
"10 - 3 = 7"   10 3 - 7 =   test
"4 * 5 = 20"   4 5 * 20 =   test
"10 / 2 = 5"   10 2 / 5 =   test
"10 mod 3 = 1" 10 3 mod 1 = test

\ сравнение
"3 = 3"  3 3 =      test
"3 != 4" 3 4 = not  test
"3 < 4"  3 4 <      test
"4 > 3"  4 3 >      test

\ стек
"swap" 1 2 swap drop 2 = test
"dup"    5 dup =         test
"over" 1 2 over drop drop 1 = test
"drop"   1 2 drop 1 =    test
"nip"    1 2 nip 2 =     test

\ математика
"abs -5"   -5 abs 5 =    test
"min 3 5"  3 5 min 3 =   test
"max 3 5"  3 5 max 5 =   test
"negate 3" 3 negate -3 = test

\ слова
: square dup * ;
"5 square = 25" 5 square 25 = test

\ цикл
variable x
0 x !
0 begin 1 + dup x ! dup 10 - until drop
"loop to 10" x @ 11 = test

results
bye
