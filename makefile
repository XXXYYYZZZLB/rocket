##################################
# makefile
##################################

PATH_BIN = bin
PATH_LIB = lib
PATH_OBJ = obj

PATH_ROCKET = rocket
PATH_COMM = $(PATH_ROCKET)/common
PATH_NET = $(PATH_ROCKET)/net

PATH_TESTCASES = testcases

PATH_INSTALL_LIB_ROOT = /usr/lib		# will install lib to /usr/lib/libsocket.a
PATH_INSTALL_INC_ROOT = /usr/include	# will install all header file to /usr/include/socket
PATH_INSTALL_INC_COMM = $(PATH_INSTALL_INC_ROOT)/$(PATH_COMM)
PATH_INSTALL_INC_NET = $(PATH_INSTALL_INC_ROOT)/$(PATH_NET)


# PATH_PROTOBUF = /usr/include/google
# PATH_TINYXML = /usr/include/tinyxml

CXX := g++

CXXFLAGS += -g -O0 -std=c++11 -Wall -Wno-deprecated -Wno-unused-but-set-variable # 以“-Wno-”开头关闭特定的警告
CXXFLAGS += -I./ -I$(PATH_ROCKET) -I$(PATH_COMM) -I$(PATH_NET) 	# 添加include

LIBS += /usr/local/lib/libprotobuf.a /usr/lib/libtinyxml.a		# 静态链接库


COMM_OBJ := $(patsubst $(PATH_COMM)/%.cpp, $(PATH_OBJ)/%.o, $(wildcard $(PATH_COMM)/*.cpp))
NET_OBJ  := $(patsubst $(PATH_NET)/%.cpp, $(PATH_OBJ)/%.o, $(wildcard $(PATH_NET)/*.cpp))


##################################
# 规则 没有命令 是为了生成多个目标
##################################
ALL_TESTS : $(PATH_BIN)/test_log $(PATH_BIN)/test_eventloop 


TEST_CASE_OUT := $(PATH_BIN)/test_log $(PATH_BIN)/test_eventloop
LIB_OUT := $(PATH_LIB)/librocket.a

$(PATH_BIN)/test_log: $(LIB_OUT) $(PATH_TESTCASES)/test_log.cpp
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_log.cpp -o $@ $(LIB_OUT) $(LIBS) -ldl -pthread
$(PATH_BIN)/test_eventloop: $(LIB_OUT) $(PATH_TESTCASES)/test_eventloop.cpp
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_eventloop.cpp -o $@ $(LIB_OUT) $(LIBS) -ldl -pthread


$(LIB_OUT): $(COMM_OBJ) $(NET_OBJ)
	cd $(PATH_OBJ) && ar rcv librocket.a *.o && cp librocket.a ../lib/

$(PATH_OBJ)/%.o : $(PATH_COMM)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 
# $@ 有几个目标文件就生成几句
# $< 依赖目标中的第一个目标名字。如果依赖目标是以模式（即"%"）定义的，那么"$<"将是符合模式的一系列的文件集


$(PATH_OBJ)/%.o : $(PATH_NET)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# print something test
# like this: make PRINT-PATH_BIN, and then will print variable PATH_BIN
PRINT-% : ; @echo $* = $($*)


# to clean 
clean :
	rm -f $(COMM_OBJ) $(NET_OBJ) $(TESTCASES) $(TEST_CASE_OUT) $(PATH_LIB)/librocket.a $(PATH_OBJ)/librocket.a

# install
install:
	mkdir -p $(PATH_INSTALL_INC_COMM) $(PATH_INSTALL_INC_NET) \
		&& cp $(PATH_COMM)/*.h $(PATH_INSTALL_INC_COMM) \
		&& cp $(PATH_NET)/*.h $(PATH_INSTALL_INC_NET) \
		&& cp $(LIB_OUT) $(PATH_INSTALL_LIB_ROOT)/


# uninstall
uninstall:
	rm -rf $(PATH_INSTALL_INC_ROOT)/ROCKET && rm -f $(PATH_INSTALL_LIB_ROOT)/librocket.a