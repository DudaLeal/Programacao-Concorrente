package main

import (
	"fmt"
	"math"
	"sync"
)

// Função para verificar se um número é primo
func ehPrimo(n int) int {
	if n <= 1 {
		return 0
	}
	if n == 2 {
		return 1
	}
	if n%2 == 0 {
		return 0
	}
	for i := 3; i <= int(math.Sqrt(float64(n)))+1; i += 2 {
		if n%i == 0 {
			return 0
		}
	}
	return 1
}

// Função para cada goroutine testar primalidade
func checaPrimo(numeros <-chan int, resultado chan<- int, wg *sync.WaitGroup) {
	defer wg.Done()
	for n := range numeros {
		resultado <- ehPrimo(n)
	}
}

func main() {
	N := 100 // Defina o valor de N (exemplo: 100)
	M := 4   // Defina o número de goroutines (exemplo: 4)

	numeros := make(chan int, N)
	resultado := make(chan int, N)
	var wg sync.WaitGroup

	for i := 0; i < M; i++ {
		wg.Add(1)
		go checaPrimo(numeros, resultado, &wg)
	}

	// Envia os números de 1 a N para o canal numeros
	go func() {
		for i := 1; i <= N; i++ {
			numeros <- i
		}
		close(numeros)
	}()

	// Aguarda todas as goroutines terminarem e fecha o canal resultado
	go func() {
		wg.Wait()
		close(resultado)
	}()

	// Conta o número total de primos encontrados
	totalPrimos := 0
	for primo := range resultado {
		totalPrimos += primo
	}

	fmt.Printf("Total de números primos entre 1 e %d: %d\n", N, totalPrimos)
}
