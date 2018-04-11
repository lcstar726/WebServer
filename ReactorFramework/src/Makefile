CXX = g++
OriClientTestBin = OriClientTest
ClientTestBin = ClientTest
ServerTestBin = ServerTest
ReactorServerTestBin = ReactorServerTest
BINPATH = ../bin

INCLUDE = . ../include \
          /usr/include

DEPENDLIBS = -L/usr/lib -lm -lpthread -lrt

override CXXFLAGS += -g -Wall
override CXXFLAGS += $(patsubst %, -I%, $(INCLUDE))
override CXXFLAGS += $(DEPENDLIBS)

ORICLTOBJ = OrigionClientTest.o
CLTOBJ = ClientTest.o
SVROBJ = ServerTest.o
ReactorSVROBJ = ReactorServerTest.o
PUBFILEOBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all:$(OriClientTestBin) $(ClientTestBin) $(ServerTestBin) $(ReactorServerTestBin)
$(OriClientTestBin): $(PUBFILEOBJS) $(ORICLTOBJ)
	$(CXX) -o $@ $? $(CXXFLAGS)
	#mv $(OriClientTestBin) $(BINPATH)/$(OriClientTestBin)

$(ClientTestBin): $(PUBFILEOBJS) $(CLTOBJ)
	$(CXX) -o $@ $? $(CXXFLAGS)
	#mv $(ClientTestBin) $(BINPATH)/$(ClientTestBin)

$(ServerTestBin): $(PUBFILEOBJS) $(SVROBJ)
	$(CXX) -o $@ $? $(CXXFLAGS)
	#mv $(ServerTestBin) $(BINPATH)/$(ServerTestBin)

$(ReactorServerTestBin): $(PUBFILEOBJS) $(ReactorSVROBJ)
	$(CXX) -o $@ $? $(CXXFLAGS)

.SUFFIXES: .c .cpp .C .hpp .h .o
.cpp.o:
	$(CXX) -c $< $(CXXFLAGS)
.c.o:
	$(CXX) -c $< $(CXXFLAGS)

.PHONY: clean
clean:
	-$(RM) -f *.o *core*
	-$(RM) -f ./$(OriClientTestBin) *core*      
	-$(RM) -f ./$(ClientTestBin) *core*         
	-$(RM) -f ./$(ServerTestBin) *core*         
	-$(RM) -f ./$(ReactorServerTestBin) *core*
	-$(RM) -f $(BINPATH)/$(OriClientTestBin) *core*
	-$(RM) -f $(BINPATH)/$(ClientTestBin) *core*
	-$(RM) -f $(BINPATH)/$(ServerTestBin) *core*
	-$(RM) -f $(BINPATH)/$(ReactorServerTestBin) *core*

