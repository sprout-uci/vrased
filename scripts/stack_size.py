
max_stack_size = 0
max_line = None
stack_ptr_base = 0x1000
with open('out_x600.txt', 'r') as f:
	line = f.readline()
	while line:
		line = f.readline()
		if 'r1' not in line:
			continue
		idx = line.find('r1')
		stack_ptr = int(line[idx+5:idx+9], 16)
		if stack_ptr > stack_ptr_base or stack_ptr == 0:
			continue
		stack_size = stack_ptr_base-stack_ptr
		if stack_size > max_stack_size:
			max_stack_size = stack_size
			max_line = line

print max_stack_size
print max_line
			


