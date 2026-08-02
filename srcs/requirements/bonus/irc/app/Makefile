NAME := ircserv

SRCS := src/main.cpp \
		src/Commands_Lead/channel.cpp \
		src/Commands_Lead/ConnectionCommands.cpp \
		src/Commands_Lead/Dispatcher.cpp \
		src/Commands_Lead/parser.cpp \
		src/Commands_Lead/ChannelCommands/Invite.cpp \
		src/Commands_Lead/ChannelCommands/Join.cpp \
		src/Commands_Lead/ChannelCommands/Kick.cpp \
		src/Commands_Lead/ChannelCommands/Privmsg.cpp \
		src/Commands_Lead/ChannelCommands/Topic.cpp \
		src/Commands_Lead/ChannelCommands/Part.cpp \
		src/Commands_Lead/ChannelCommands/Mode.cpp \
		src/Infrastructure_lead/Client.cpp \
		src/Infrastructure_lead/Server.cpp \
		src/Infrastructure_lead/Network.cpp \

OBJS := $(SRCS:.cpp=.o)

INCLUDES = -I./includes

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all : $(NAME)

re : fclean all

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)
