BLDDIR     = build
LIBDIR     = lib
INCLUDEDIR = include/spica
TARGET = libspica.a

DEPFLAGS = -MT $@ -MMD -MP -MF $(BLDDIR)/$*.Td
CFLAGS += -Wall -pedantic -std=c11 -O2 -march=native -mtune=native

.PHONY: all clean doc

SRCS = \
	SImage.c \
	SImageIO.c

PUBLIC_HDRS = \
	SCommon.h \
	SImage.h \
	SImageIO.h \
	SVec.h

OBJS=$(patsubst %, $(BLDDIR)/%.o, $(basename $(SRCS)))
PUBHDRS=$(patsubst %, $(INCLUDEDIR)/%, $(PUBLIC_HDRS))

all: $(LIBDIR)/$(TARGET) $(PUBHDRS)

doc:
	doxygen

$(INCLUDEDIR):
	mkdir -p $@

$(LIBDIR):
	mkdir -p $@

$(BLDDIR):
	mkdir -p $@

$(LIBDIR)/$(TARGET): $(OBJS) | $(LIBDIR)
	ar rvs $@ $^

$(INCLUDEDIR)/%.h: src/%.h | $(INCLUDEDIR)
	cp $< $@

$(BLDDIR)/%.o: src/%.c $(BLDDIR)/%.d | $(BLDDIR)
	$(CC) $(DEPFLAGS) $(CFLAGS) -o $@ -c $<
	mv -f $(BLDDIR)/$*.Td $(BLDDIR)/$*.d

$(BLDDIR)/%.d: $(BLDDIR) ;
.PRECIOUS: $(BLDDIR)/%.d

include $(wildcard $(patsubst %, $(BLDDIR)/%.d, $(basename $(SRCS))))

clean:
	rm -f $(BLDDIR)/*.o $(BLDDIR)/*.d
	rmdir $(BLDDIR)
	rm -f $(INCLUDEDIR)/*.h
	rmdir $(INCLUDEDIR)
	rm -f $(LIBDIR)/$(TARGET)
	rmdir $(LIBDIR)
