/// PROJECT:        ft_irc
/// FILE:           num_responses.hpp
/// AUTHOR:         Nathaniel Jackson
/// DATE CREATED:   2024-11-25

#pragma once
#include <sstream>
#include <ostream>

/* I hate iostreams >:( */

#define IRC_NUMREPLY(reply) ((std::ostringstream&)(std::ostringstream("").flush() << reply)).str()

#define ERR_UNKNOWNCOMMAND(command)         IRC_NUMREPLY("421 " << command << " :Unknown command")

// topic
#define RPL_NOTOPIC(channel)                IRC_NUMREPLY("331 " << channel << " :No topic is set")
#define RPL_TOPIC(channel, topic)           IRC_NUMREPLY("332 " << channel << " :" << topic)

// invites
#define RPL_INVITING(channel, nick)         IRC_NUMREPLY("341 " << channel << " " << nick)
#define ERR_USERONCHANNEL(user, channel)    IRC_NUMREPLY("443 " << user << " " << channel << " :User already on channel")

// regestering
#define ERR_NOSUCHNICK                      std::string("401 :No such nickname/channel")
#define ERR_NONICKNAMEGIVEN                 std::string("431 :No nickname given")
#define ERR_ERRONEUSNICKNAME(nick)          IRC_NUMREPLY("432 " << nick << " :Erroneus nickname")
#define ERR_NICKNAMEINUSE(nick)             IRC_NUMREPLY("433 " << nick << " :Nickname is already in use")
#define ERR_NICKCOLLISION(nick)             IRC_NUMREPLY("436 " << nick << " :Nickname collision KILL")
#define ERR_ALREADYREGISTRED                std::string("462 :You may not register")

// modes
#define RPL_CHANNELMODEIS(channel, mode, mode_params)   IRC_NUMREPLY("324 " << channel << " " << mode << " " << mode_params)
#define ERR_KEYSET(channel)                 IRC_NUMREPLY("467 " << channel << " :Channel key already set")
#define ERR_CHANNELISFULL(channel)          IRC_NUMREPLY("471 " << channel << " :Cannot join channel (+l)")
#define ERR_UNKNOWNMODE(chara)              IRC_NUMREPLY("472 " << chara << " :is unknown mode char to me")
#define ERR_INVITEONLYCHAN(channel)         IRC_NUMREPLY("473 " << channel << " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(channel)          IRC_NUMREPLY("475 " << channel << " :Cannot join channel (+k)")

// general
#define ERR_NOSUCHCHANNEL(channel)          IRC_NUMREPLY("403 " << channel << " :No such channel")
#define ERR_CANNOTSENDTOCHAN(channel)       IRC_NUMREPLY("404 " << channel << " :Cannot send to channel")
#define ERR_NORECIPIENT(command)            IRC_NUMREPLY("411 :No recipient given " << command)
#define ERR_NOTEXTTOSEND                    std::string("412 :No text to send")
#define ERR_NOTONCHANNEL(channel)           IRC_NUMREPLY("442 " << channel << " :You're not on that channel")
#define ERR_NEEDMOREPARAMS(command)         IRC_NUMREPLY("461 " << command << " :You may not reregister")
#define ERR_CHANOPRIVSNEEDED(channel)       IRC_NUMREPLY("482 " << channel << " :You're not channel operator")

// MOTD
#define RPL_MOTDSTART(msg)  IRC_NUMREPLY("375 :- " << msg)
#define RPL_MOTD(msg)       IRC_NUMREPLY("372 :- " << msg)
#define RPL_ENDOFMOTD(msg)  IRC_NUMREPLY("376 :End of /MOTD command")

/*
RPL_CHANNELMODEIS --    324 <channel> <mode> <mode params>
RPL_NOTOPIC --          331 <channel> :No topic is set
RPL_TOPIC --            332 <channel> :<topic>
RPL_INVITING --         341 <channel> <nick>
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
