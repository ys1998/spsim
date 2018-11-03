

#ifndef _FLUSHER_HPP
#define _FLUSHER_HPP

#include "instruction.hpp"
#include "fetcher.hpp"
#include "register.hpp"
#include "lists.hpp"
#include "decoder.hpp"
#include "queues.hpp"
#include "issuer.hpp"
#include "latch.hpp"
#include "writer.hpp"

class Flusher {
protected:
	Issuer *is;
	Fetcher *f;
	Decoder *d;
	Writer *w;

public:
	Flusher(Issuer* , Fetcher* , Decoder* , Writer* );
	void flush(int,int);
};

#endif