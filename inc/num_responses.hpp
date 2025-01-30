/// PROJECT:        ft_irc
/// FILE:           num_responses.hpp
/// AUTHOR:         Nathaniel Jackson
/// DATE CREATED:   2024-11-25

#pragma once
#include <sstream>
#include <ostream>

/* I hate iostreams >:( */

#define IRC_NUMREPLY(reply) ((std::ostringstream&)(std::ostringstream("").flush() << ":ft_irc " << reply)).str()

#define ERR_UNKNOWNCOMMAND(user, command)       IRC_NUMREPLY("421 " << user << " " << command << " :Unknown command")

// topic
#define RPL_NOTOPIC(user, channel)              IRC_NUMREPLY("331 " << user << " " << channel << " :No topic is set")
#define RPL_TOPIC(user, channel, topic)         IRC_NUMREPLY("332 " << user << " " << channel << " :" << topic)

// joining
#define RPL_NAMREPLY(user, channel, nicks)      IRC_NUMREPLY("353 " << user << " ahah " << channel << " :" << nicks)
#define RPL_ENDOFNAMES(user, channel)           IRC_NUMREPLY("366 " << user << " " << channel << " :End of /NAMES list")

// invites
#define RPL_INVITING(user, channel, nick)       IRC_NUMREPLY("341 " << user << " " << channel << " " << nick)
#define ERR_USERONCHANNEL(user, nick, channel)  IRC_NUMREPLY("443 " << user << " " << nick << " " << channel << " :User already on channel")

// regestering
#define ERR_NOSUCHNICK(user)                    IRC_NUMREPLY("401 " << user << " :No such nickname/channel")
#define ERR_NONICKNAMEGIVEN(user)               IRC_NUMREPLY("431 " << user << " :No nickname given")
#define ERR_ERRONEUSNICKNAME(user, nick)        IRC_NUMREPLY("432 " << user << " " << nick << " :Erroneus nickname")
#define ERR_NICKNAMEINUSE(user, nick)           IRC_NUMREPLY("433 " << user << " " << nick << " :Nickname is already in use")
#define ERR_NICKCOLLISION(user, nick)           IRC_NUMREPLY("436 " << user << " " << nick << " :Nickname collision KILL")
#define ERR_ALREADYREGISTRED(user)              IRC_NUMREPLY("462 " << user << " :You may not register")

// modes
#define RPL_CHANNELMODEIS(user, channel, mode, details)        IRC_NUMREPLY("324 " << user << " " << channel << " " << mode << " " << details)
#define ERR_KEYSET(user, channel)               IRC_NUMREPLY("467 " << user << " " << channel << " :Channel key already set")
#define ERR_CHANNELISFULL(user, channel)        IRC_NUMREPLY("471 " << user << " " << channel << " :Cannot join channel (+l)")
#define ERR_UNKNOWNMODE(user, chara)            IRC_NUMREPLY("472 " << user << " " << chara << " :is unknown mode char to me")
#define ERR_INVITEONLYCHAN(user, channel)       IRC_NUMREPLY("473 " << user << " " << channel << " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(user, channel)        IRC_NUMREPLY("475 " << user << " " << channel << " :Cannot join channel (+k)")

// general
#define ERR_NOSUCHCHANNEL(user, channel)        IRC_NUMREPLY("403 " << user << " " << channel << " :No such channel")
#define ERR_CANNOTSENDTOCHAN(user, channel)     IRC_NUMREPLY("404 " << user << " " << channel << " :Cannot send to channel")
#define ERR_NORECIPIENT(user, command)          IRC_NUMREPLY("411 " << user << " :No recipient given " << command)
#define ERR_NOTEXTTOSEND(user)                  IRC_NUMREPLY("412 " << user << " :No text to send")
#define ERR_USERNOTINCHANNEL(user, nick, channel)    IRC_NUMREPLY("441 " << user << " " << nick << " " << channel << " :They aren't on that channel")
#define ERR_NOTONCHANNEL(user, channel)         IRC_NUMREPLY("442 " << user << " " << channel << " :You're not on that channel")
#define ERR_NEEDMOREPARAMS(user, command)       IRC_NUMREPLY("461 " << user << " " << command << " :Not enough parameters")
#define ERR_CHANOPRIVSNEEDED(user, channel)     IRC_NUMREPLY("482 " << user << " " << channel << " :You're not channel operator")

// MOTD
#define RPL_MOTDSTART(user, msg)  IRC_NUMREPLY("375 " << user << " :- " << msg)
#define RPL_MOTD(user, msg)       IRC_NUMREPLY("372 " << user << " :- " << msg)
#define RPL_ENDOFMOTD(user, msg)  IRC_NUMREPLY("376 " << user << " :End of /MOTD command")

/*
RPL_CHANNELMODEIS --    324 <channel> <mode> <mode params>
RPL_NOTOPIC --          331 <channel> :No topic is set
RPL_TOPIC --            332 <channel> :<topic>
RPL_INVITING --         341 <channel> <nick>
RPL_NAMREPLY --         353 <channel> :<nick>[ <nick>...]
RPL_ENDOFNAMES --       366 <channel> :End of /NAMES list
ERR_NOSUCHNICK --       401 :No such nickname/channel
ERR_NOSUCHCHANNEL --    403 <channel name> :No such channel
ERR_CANNOTSENDTOCHAN -- 404 <channel name> :Cannot send to channel
ERR_NORECIPIENT --      411 :No recipient given <command>
ERR_NOTEXTTOSEND --     412 :No text to send
ERR_NONICKNAMEGIVEN --  431 :No nickname given
ERR_ERRONEUSNICKNAME -- 432 <nick> :Erroneus nickname
ERR_NICKNAMEINUSE --    433 <nick> :Nickname is already in use
ERR_NICKCOLLISION --    436 <nick> :Nickname collision KILL // maybe not used?
ERR_NOTONCHANNEL --     442 <channel> :You're not on that channel
ERR_USERONCHANNEL --    443 <user> <channel> :User already on channel
ERR_NEEDMOREPARAMS --   461 <command> :Not enough parameters
ERR_ALREADYREGISTRED -- 462 :You may not reregister
ERR_KEYSET --           467 <channel> :Channel key already set
ERR_CHANNELISFULL --    471 <channel> :Cannot join channel (+l)
ERR_UNKNOWNMODE --      472 <char> :is unknown mode char to me
ERR_INVITEONLYCHAN --   473 <channel> :Cannot join channel (+i)
ERR_BADCHANNELKEY --    475 <channel> :Cannot join channel (+k)
ERR_CHANOPRIVSNEEDED -- 482 <channel> :You're not channel operator
*/
