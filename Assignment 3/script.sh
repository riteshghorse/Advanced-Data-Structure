# diff input.bin.000 basic/input.bin.000  
# diff input.bin.007 basic/input.bin.007  
# diff input.bin.014 basic/input.bin.014  
# diff input.bin.021 basic/input.bin.021  
# diff input.bin.001 basic/input.bin.001   
# diff input.bin.008 basic/input.bin.008   
# diff input.bin.015 basic/input.bin.015   
# diff input.bin.022 basic/input.bin.022  
# diff input.bin.002 basic/input.bin.002  
# diff input.bin.009 basic/input.bin.009  
# diff input.bin.016 basic/input.bin.016  
# diff input.bin.023 basic/input.bin.023  
# diff input.bin.003 basic/input.bin.003 
# diff input.bin.010 basic/input.bin.010 
# diff input.bin.017 basic/input.bin.017  
# diff input.bin.024 basic/input.bin.024 
# diff input.bin.004 basic/input.bin.004  
# diff input.bin.011 basic/input.bin.011  
# diff input.bin.018 basic/input.bin.018  
# diff input.bin.005 basic/input.bin.005  
# diff input.bin.012 basic/input.bin.012  
# diff input.bin.019 basic/input.bin.019  
# diff input.bin.006 basic/input.bin.006  
# diff input.bin.013 basic/input.bin.013
# diff input.bin.020 basic/input.bin.020  

gcc dump.c -o dump
gcc assn_3.c -o assn_3
./assn_3 --basic input.bin sort.bin 
diff sort.bin op_sort.bin 
./dump sort.bin > sort.txt
diff -c sort.txt op_sort.txt > output_basic.txt