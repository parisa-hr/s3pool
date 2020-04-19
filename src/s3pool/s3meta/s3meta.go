package s3meta

import (
	"hash/fnv"
)


type requestType struct {
	command string
	param []string
	reply chan *replyType
}


type replyType struct {
	err error
	key []string
	etag []string
}


type serverCB struct {
	ch chan *requestType
}

var server []*serverCB
var nserver uint32
var store storeCB

func newServer() *serverCB {
	s := &serverCB{make(chan *requestType)}
	go s.run()
	return s
}

func Initialize(n int) {
	if (n <= 0) {
		n = 29
	}
	nserver = uint32(n)
	server = make([]*serverCB, n)
	for i := 0; i < n; i++ {
		server[i] = newServer()
	}
}

func KnownBuckets() []string {
	return getKnownBuckets()
}

func SearchExact(bucket, key string) (etag string) {
	return searchExact(bucket, key)
}


func List(bucket string, prefix string) (error, []string, []string) {
	ch := make(chan *replyType)
	h := fnv.New32a()
	h.Write([]byte(bucket))
	h.Write([]byte{0})
	h.Write([]byte(prefix))	
	server[h.Sum32() % nserver].ch <- &requestType{"LIST", []string{bucket, prefix}, ch}
	reply := <- ch
	return reply.err, reply.key, reply.etag
}
