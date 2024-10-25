package main

import (
	"fmt"
)

func tarefa(str chan string) {
	// Recebe mensagem da main
	msg := <-str
	fmt.Println("Goroutine recebeu:", msg)

	// Envia resposta para main
	str <- "Oi Main, bom dia, tudo bem?"

	// Recebe a próxima mensagem da main
	msg = <-str
	fmt.Println("Goroutine recebeu:", msg)

	// Envia resposta final para main
	str <- "Certo, entendido."

	// Finalizando a goroutine
	fmt.Println("Goroutine: finalizando")
}

func main() {
	// Cria um canal de comunicação
	str := make(chan string)

	// Cria uma goroutine que executará a função 'tarefa'
	go tarefa(str)

	// Main envia mensagem para goroutine
	str <- "Olá, Goroutine, bom dia!"

	// Main recebe resposta da goroutine
	msg := <-str
	fmt.Println("Main recebeu:", msg)

	// Main envia outra mensagem para goroutine
	str <- "Tudo bem! Vou terminar tá?"

	// Main recebe resposta final da goroutine
	msg = <-str
	fmt.Println("Main recebeu:", msg)

	// Finalizando main
	fmt.Println("Main: finalizando")
}
