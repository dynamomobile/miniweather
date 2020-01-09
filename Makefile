.SILENT:

all:
	echo "make clean"
	echo "make build"
	echo "make install"

clean:
	pio run --target clean
	rm -rfv .pio

build:
	pio run

install:
	pio run --target upload
