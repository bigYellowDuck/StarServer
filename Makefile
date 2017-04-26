AR = ar
CXX = g++
CXXFLAGS =-I. -O2 -std=c++11 
LDFFLAGS = -pthread

STAR_SOURCES += ${shell find star -name '*.cpp'}
STAR_OBJECTS = ${STAR_SOURCES:.cpp=.o}      # 把xx.cpp替换成xx.o

EXAMPLE_SOURCES += ${shell find examples -name '*.cpp'}
EXAMPLES = ${EXAMPLE_SOURCES:.cpp=}         # 把xx.cpp替换成xx

LIBRARY = libstar.a

TARGETS = $(LIBRARY) ${EXAMPLES}

default: $(TARGETS)
${EXAMPLES} : ${LIBRARY}

install: libstar.a
	mkdir -p /usr/local/include/star
	cp -f star/*.h /usr/local/include/star
	cp -f libstar.a /usr/local/lib

uninstall: 
	rm -rf /usr/local/include/star /usr/local/lib/libstar.a

clean:
	-rm -f $(TARGETS)
	-rm -f */*.o

$(LIBRARY): $(STAR_OBJECTS)           # 使用所有的.o文件生成静态库
	rm -f $@						  # 删除原来的目标文件
		$(AR) -rs $@ $(STAR_OBJECTS)  # 生成静态库

$(STAR_OBJECTS): %.o:%.cpp            # 对每个.cpp文件生成.o文件
	$(CXX) -c $(CXXFLAGS) $< -o $@

.cpp:
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFFLAGS) $(LIBRARY)

