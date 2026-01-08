from math import log2, ceil

class BuddyAllocator:
    def __init__(self, memory_size):
        self.memory_size = memory_size
        self.divide_limit = self.calculate_divide_limit(memory_size)
        self.divide_limit_list = self.calculate_divide_limit_list(memory_size)

    def calculate_divide_limit(self, size):
        if size < 32 or size > 2048:
            raise ValueError("Memory size must be at least 32 bytes and at most 2048 bytes.")
        if size & (size - 1) != 0:
            raise ValueError("Memory size must be a power of two.")
        divide_limit = 0
        while size > 32:
            size = size // 2
            divide_limit += 1
        return divide_limit
    
    def calculate_divide_limit_list(self, size):
        divide_limit_list = [size]
        while size > 32:
            size = size // 2
            divide_limit_list.append(size)
        return divide_limit_list
    
    def alloc(self):
        block_size = 2 ** ceil(log2(self.memory_size))
        if block_size in self.divide_limit_list:
            index = self.divide_limit_list.index(block_size)
            self.divide_limit_list[index] = 0
            alloc_address = index * block_size
            return [alloc_address, block_size]
        else:
            for i in range(len(self.divide_limit_list)):
                if self.divide_limit_list[i] >= block_size:
                    alloc_address = i * self.divide_limit_list[i]
                    while self.divide_limit_list[i] > block_size:
                        half_size = self.divide_limit_list[i] // 2
                        self.divide_limit_list[i] = half_size
                        self.divide_limit_list.append(half_size)
                    self.divide_limit_list[i] = 0
                    return [alloc_address, block_size]
    
    def free(self):
        return 

if __name__ == "__main__":
    allocator = BuddyAllocator(2048)
    print(f"Divide limit for memory size {allocator.memory_size} is {allocator.divide_limit}")
    print(f"Allocating memory blocks: {allocator.divide_limit_list}")

    print(allocator.alloc())
