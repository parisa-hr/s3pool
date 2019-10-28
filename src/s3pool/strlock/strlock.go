package strlock

import (
	"fmt"
	"sync"
	"time"
)

var tabmux sync.Mutex
var tabcond *sync.Cond
var tab = map[string]bool{}

func init() {
	tabcond = sync.NewCond(&tabmux)
}

func Lock(s string) (*string, error) {
	tabmux.Lock()
	defer tabmux.Unlock()
	for tab[s] {
		tabcond.Wait()
	}
	tab[s] = true
	return &s, nil
}

func Unlock(s *string) {
	tabmux.Lock()
	defer tabmux.Unlock()
	delete(tab, *s)
	tabcond.Broadcast()
}

func Test() {
	f := func(id int) {
		key, _ := Lock("abcd")
		time.Sleep(time.Second * 2)
		fmt.Println(id, " up")
		Unlock(key)
	}
	for i := 1; i < 10; i++ {
		go f(i)
	}

	t := time.NewTicker(time.Second)
	for i := 1; i < 100; i++ {
		select {
		case <-t.C:
			fmt.Printf("tick %d\n", i)
		}
	}
}
