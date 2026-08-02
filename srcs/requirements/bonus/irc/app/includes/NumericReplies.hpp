#ifndef NUMERICREPLIERS_HPP

#define NUMERICREPLIERS_HPP

#define RPL_WELCOME "001" //Sent after successful PASS/NICK/USER registration.

#define ERR_NOSUCHNICK "401" //Target of PRIVMSG or KICK does not exist.

#define ERR_UNKNOWNCOMMAND "421" //Command received is not in your Dispatcher map.

#define ERR_NONICKNAMEGIVEN "431" //NICK command received without a name parameter.
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433" //User tried to take a nickname already on the server.

#define ERR_NOTREGISTERED "451" // User tried a command before finishing the handshake.

#define ERR_NEEDMOREPARAMS "461" // Command received with too few parameters.

#define ERR_ALREADYREGISTRED "462" //User tried to send USER command twice.

#define ERR_PASSWDMISMATCH "464" // Password incorrect


// --- CHANNEL ERRORS ---
#define ERR_NOSUCHCHANNEL "403"
#define ERR_NOTONCHANNEL "442"
#define ERR_CHANNELISFULL "471"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BADCHANNELKEY "475"
#define ERR_CHANOPRIVSNEEDED "482"

// --- CHANNEL REPLIES ---
#define RPL_NOTOPIC "331"
#define RPL_TOPIC "332"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"

// PRIVMSG
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_NOSUCHNICK "401"
#define ERR_CANNOTSENDTOCHAN "404"

// TOPIC

#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_USERNOTINCHANNEL "441"

#define RPL_INVITING "341"
#define ERR_USERONCHANNEL "443"

#define ERR_NOORIGIN "409"

#define RPL_CHANNELMODEIS "324"
#define ERR_UNKNOWNMODE "472"


#endif