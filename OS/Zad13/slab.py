class Slab:
    def __init__(self, start_address, object_size, objects_per_slab):
        self.object_size = object_size
        self.objects_per_slab = objects_per_slab
        self.start_address = start_address

        # Obliczenie końca adresu slaba
        self.end_address = start_address + (object_size + objects_per_slab)

        # Symulacja ciągłej pamięci (lista bajtów)
        self.memory = bytearray(object_size * objects_per_slab)

        # Mapa bitowa zajętości
        self.bitmap = [False] * objects_per_slab
        print(f"[SLAB] Utworzono nowy slab. Adresy: {self.start_address} - {self.end_address}")

    def is_full(self):
        return False not in self.bitmap

    def owns_address(self, ptr):
        # Czy wskaźnik mieści się w adresie slaba
        return self.start_address <= ptr < self.end_address

    def alloc(self):
        if self.is_full():
            return None
        # Pierwsze wolne miejsce
        free_index = self.bitmap.index(False)
        self.bitmap[free_index] = True

        # Adres pamięci do zwrócenia
        address = self.start_address + (free_index * self.object_size)
        return address

    def free(self, ptr):
        # Który index
        offset = ptr - self.start_address
        index = offset // self.object_size

        if self.bitmap[index] is False:
            print(f"Błąd: Próba zwolnienia wolnego obiektu: {ptr}")
            return

        self.bitmap[index] = False
        print(f"Zwolniono obiekt pod adresem {ptr}")

class SlabCache:
    def __init__(self, object_size, objects_per_slab=5):
        self.object_size = object_size
        self.objects_per_slab = objects_per_slab
        self.slabs = []

        self._next_slab_start_addr = 0

    def alloc(self):
        print(f"- Żądanie alokacji obiektu o rozmiarze {self.object_size} -")

        target_slab = None
        for slab in self.slabs:
            if not slab.is_full():
                target_slab = slab
                break

        if target_slab is None:
            print(f"Brak miejsca, tworzenie nowego slaba...")
            new_slab = Slab(self._next_slab_start_addr, self.object_size, self.objects_per_slab)
            self.slabs.append(new_slab)
            target_slab = new_slab

            self._next_slab_start_addr += (self.object_size * self.objects_per_slab)

        ptr = target_slab.alloc()
        print(f"Zalokowano pod adresem {ptr}")
        return ptr

    def free(self, ptr):
        print(f"- Żądanie zwolnienia adresu {ptr} -")
        found_slab = None
        for slab in self.slabs:
            if slab.owns_address(ptr):
                found_slab = slab
                break

        if found_slab:
            found_slab.free(ptr)
        else:
            print(f"Error: Nie znaleziono slaba dla adresu {ptr}")

if __name__ == "__main__":
    cache = SlabCache(object_size=64, objects_per_slab=3)

    ptr1 = cache.alloc() # Adres 0
    ptr2 = cache.alloc() # Adres 64
    ptr3 = cache.alloc() # Adres 128

    print("\nStatus: Zapełnienie pierwszego slaba.\n")

    ptr4 = cache.alloc() # Adres 192 (nowy slab)
    cache.free(ptr2)
    ptr_new = cache.alloc() # Adres 64
    cache.free(ptr4)
