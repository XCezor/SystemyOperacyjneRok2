#!/usr/bin/env python3
import sys


class Process:
    """Klasa reprezentująca proces w systemie operacyjnym."""
    
    def __init__(self, name, length, start):
        self.name = name
        self.length = length  # Całkowity czas wykonania
        self.start = start    # Czas pojawienia się w systemie
        self.remaining = length  # Pozostały czas wykonania


class RoundRobinScheduler:
    """Symulator planisty Round Robin."""
    
    def __init__(self, csv_path, quantum):
        self.quantum = quantum
        self.waiting = self._load_processes(csv_path)  # Kolejka procesów oczekujących
        self.ready = []  # Kolejka procesów gotowych do wykonania
        self.time = 0
    
    def _load_processes(self, csv_path):
        """Wczytuje procesy z pliku CSV."""
        processes = []
        with open(csv_path, 'r') as f:
            for line in f:
                name, length, start = line.strip().split(',')
                processes.append(Process(name, int(length), int(start)))
        return processes
    
    def _move_arrived_processes(self):
        """Przenosi procesy z kolejki oczekującej do kolejki gotowych."""
        while self.waiting and self.waiting[0].start <= self.time:
            p = self.waiting.pop(0)
            self.ready.append(p)
            print(f"T={self.time}: New process {p.name} is waiting for execution (length={p.length})")
    
    def run(self):
        """Wykonuje symulację algorytmu Round Robin."""
        while self.waiting or self.ready:
            # Przenieś procesy, które już się pojawiły
            self._move_arrived_processes()
            
            # Jeśli brak procesów gotowych do wykonania
            if not self.ready:
                if self.waiting:
                    print(f"T={self.time}: No processes currently available")
                    self.time = self.waiting[0].start
                    continue
                else:
                    break
            
            # Pobierz proces z kolejki gotowych
            p = self.ready.pop(0)
            run_time = min(self.quantum, p.remaining)
            
            print(f"T={self.time}: {p.name} will be running for {run_time} time units. Time left: {p.remaining - run_time}")
            
            # Wykonaj proces
            p.remaining -= run_time
            self.time += run_time
            
            # Sprawdź czy proces się zakończył
            if p.remaining == 0:
                print(f"T={self.time}: Process {p.name} has been finished")
            else:
                # Sprawdź nowe procesy, które pojawiły się podczas wykonania
                self._move_arrived_processes()
                # Wywłaszcz proces i dodaj na koniec kolejki
                self.ready.append(p)
        
        print(f"T={self.time}: No more processes in queues")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python rr.py <csv_file> <quantum>")
        sys.exit(1)
    
    scheduler = RoundRobinScheduler(sys.argv[1], int(sys.argv[2]))
    scheduler.run()