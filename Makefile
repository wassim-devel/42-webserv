# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aoizel <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/06 14:50:12 by aoizel            #+#    #+#              #
#    Updated: 2024/04/08 09:47:58 by aoizel           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME			=	webserv

RAW_SOURCES		=	main.cpp VirtualServer.cpp Location.cpp WebServ.cpp Socket.cpp utils.cpp HTTPMessage.cpp Client.cpp

SOURCES_DIR		=	srcs/

SOURCES			=	$(addprefix $(SOURCES_DIR),$(RAW_SOURCES))

INCLUDES_DIR	=	includes/

CXX				=	c++

CPP_FLAGS		=	-Wall -Wextra -Werror -std=c++98

OBJECTS_DIR		=	.objs/

OBJECTS			=	$(addprefix $(OBJECTS_DIR),$(RAW_SOURCES:.cpp=.o))

DEPENDENCIES	=	Makefile

DEPS			=	$(OBJECTS:.o=.d)

CPP				=	$(CXX) $(CPP_FLAGS)


all:				$(OBJECTS_DIR) $(DEPDIR)
						make $(NAME)

$(NAME):			$(OBJECTS_DIR) $(OBJECTS)
						$(CPP) $(OBJECTS) -o $@

$(OBJECTS_DIR):
					mkdir -p $@

$(OBJECTS_DIR)%.o:	$(SOURCES_DIR)%.cpp $(DEPENDENCIES)
						$(CPP) -c -MMD $< -I$(INCLUDES_DIR) -o $@

clean:
						rm -rf $(OBJECTS_DIR)

fclean:				clean
						rm -rf $(NAME)

re:					fclean
						make all

.PHONY:				all clean fclean re

-include $(DEPS)
