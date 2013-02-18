include Makedefs

.PHONY: clean test

all: $(LIBNAME).so.$(LIBVERS).$(LIBMIN)

$(LIBNAME).so.$(LIBVERS).$(LIBMIN): *.h $(FILES)
	${CC} ${CFLAGS} $(LIBFLAGS) -I$(INCLUDEDIR) -flto $(FILES)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.$(LIBVERS) -flto -O3 \
		-o $(LIBNAME).so.$(LIBVERS).$(LIBMIN) *.o $(LDFLAGS) 
	cp $(LIBNAME).so.$(LIBVERS).$(LIBMIN) $(LIBDIR)

test: $(LIBNAME).so.$(LIBVERS).$(LIBMIN)
	make -C test all

clean:
	rm -f *.o *.lo *.la *~ *.as *.out
	rm -f $(LIBNAME).so.$(LIBVERS).$(LIBMIN)
	make -C test clean
