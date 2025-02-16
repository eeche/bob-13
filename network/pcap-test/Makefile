LDLIBS += -lpcap

all: pcap-test

pcap-test: pcap_test.c
	gcc -o pcap-test pcap_test.c $(LDLIBS)

clean:
	rm -f pcap-test *.o
