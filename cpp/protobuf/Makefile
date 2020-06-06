# See README.txt.

.PHONY: all cpp clean

all: cpp

cpp: add_person list_people

clean:
	rm -f add_person list_people
	rm -f protoc_middleman addressbook.pb.cc addressbook.pb.h
	rmdir tutorial 2>/dev/null || true

protoc_middleman: addressbook.proto
	protoc $$PROTO_PATH --cpp_out=. addressbook.proto
	@touch protoc_middleman

add_person: add_person.cc protoc_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ -std=c++17 add_person.cc addressbook.pb.cc -o add_person `pkg-config --cflags --libs protobuf`

list_people: list_people.cc protoc_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	c++ -std=c++17 list_people.cc addressbook.pb.cc -o list_people `pkg-config --cflags --libs protobuf`
