from math import log2, ceil

class BuddyAllocator:
    def __init__(self, memory_size, limit):
        if memory_size & (memory_size - 1) != 0:
            raise ValueError("Rozmiar pamięci musi być potęgą liczby 2")

        self.memory_size = memory_size
        # Minimalny rozmiar bloku na podstawie limitu podziałów
        self.min_block_size = int(memory_size / (2 ** limit))
        self.free_blocks = {}
        curr_size = self.min_block_size

        while curr_size <= self.memory_size:
            self.free_blocks[curr_size] = []
            curr_size *= 2

        self.free_blocks[self.memory_size].append(0)

    def alloc(self, size):
        if size <= 0:
            return None

        required_size = 2 ** ceil(log2(size))

        if required_size < self.min_block_size:
            required_size = self.min_block_size

        curr_size = required_size
        while curr_size <= self.memory_size:
            if len(self.free_blocks[curr_size]) > 0:
                address = self.free_blocks[curr_size].pop(0)

                while curr_size > required_size:
                    curr_size //= 2
                    buddy_address = address + curr_size
                    self.free_blocks[curr_size].append(buddy_address)

                return address, required_size
            # Nie znalazło bloku o danym rozmiarze, zwiększenie rozmiaru
            curr_size *= 2
        return None

    def free(self, address, size):
        # Zabezpieczenie przed invalid free
        if address in self.free_blocks.get(size, []):
            print(f"Błąd: Double free pod adresem {address}")
            return

        while size < self.memory_size:
            # Buddy
            buddy_address = address ^ size

            if buddy_address in self.free_blocks[size]:
                self.free_blocks[size].remove(buddy_address)
                address = min(address, buddy_address)
                size *= 2
            else:
                break
        self.free_blocks[size].append(address)

if __name__ == "__main__":
    allocator = BuddyAllocator(2048, 6)

    print(f"Stan początkowy: {allocator.free_blocks}\n")

    # Alokacja A (wymaga 100 -> blok 128)
    ptr_a, size_a = allocator.alloc(100)
    print(f"Zaalokowano A: adres={ptr_a}, rozmiar={size_a}")
    print(f"Wolne bloki: {allocator.free_blocks}\n")

    # Alokacja B (wymaga 200 -> blok 256)
    ptr_b, size_b = allocator.alloc(200)
    print(f"Zaalokowano B: adres={ptr_b}, rozmiar={size_b}")
    print(f"Wolne bloki: {allocator.free_blocks}\n")

    # Alokacja C (wymaga 50 -> blok 64)
    ptr_c, size_c = allocator.alloc(50)
    print(f"Zaalokowano C: adres={ptr_c}, rozmiar={size_c}")
    print(f"Wolne bloki: {allocator.free_blocks}\n")

    # Zwolnienie B
    print(f"Zwalniam B ({ptr_b}, {size_b})...")
    allocator.free(ptr_b, size_b)
    print(f"Wolne bloki: {allocator.free_blocks}\n")

    # Zwolnienie A i C
    print(f"Zwalniam A ({ptr_a}, {size_a}) i C ({ptr_c}, {size_c})...")
    allocator.free(ptr_a, size_a)
    allocator.free(ptr_c, size_c)
    print(f"Wolne bloki: {allocator.free_blocks}")