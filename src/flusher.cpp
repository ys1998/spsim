/*
	Class declaration for invalid instruction flushing unit.
*/

#include "flusher.hpp"

Flusher::Flusher(Issuer *is, Fetcher *f, Decoder *d, Writer *w){
	this->is = is;
	this->f = f;
	this->d = d;
	this->w = w;
}

void Flusher::flush(int id, int PC){
	is->flush(id);
	d->flush(id);
	f->update_PC(PC);
	w->flush(id);
}
