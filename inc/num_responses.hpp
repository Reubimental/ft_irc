/// PROJECT:        ft_irc
/// FILE:           num_responses.hpp
/// AUTHOR:         Nathaniel Jackson
/// DATE CREATED:   2024-11-25

#pragma once
#include <sstream>

/* I hate iostreams >:( */

#define ERR_UNKNOWNCOMMAND(command)         (std::ostringstream << "421 " << command << " :Unknown command").str()

// topic
#define RPL_NOTOPIC(channel)                (std::ostringstream << "331 " << channel << " :No topic is set").str()
#define RPL_TOPIC(channel, topic)           (std::ostringstream << "332 " << channel << " :" << topic).str()

// invites
#define RPL_INVITING(channel, nick)         (std::ostringstream << "341 " << channel << " " << nick).str()
#define ERR_USERONCHANNEL(user, channel)    (std::ostringstream << "443 " << user << " " << channel << " :User already on channel").str()

// regestering
#define ERR_NOSUCHNICK                      std::string("401 :No such nickname/channel")
#define ERR_NONICKNAMEGIVEN                 std::string("431 :No nickname given")
#define ERR_ERRONEUSNICKNAME(nick)          (std::ostringstream << "432 " << nick << " :Erroneus nickname").str()
#define ERR_NICKNAMEINUSE(nick)             (std::ostringstream << "433 " << nick << " :Nickname is already in use").str()
#define ERR_NICKCOLLISION(nick)             (std::ostringstream << "436 " << nick << " :Nickname collision KILL").str()
#define ERR_ALREADYREGISTRED                std::string("462 :You may not register")

// modes
#define RPL_CHANNELMODEIS(channel, mode, mode_params)   (std::ostringstream << "324 " << channel << " " << mode << " " << mode_params).str()
#define ERR_KEYSET(channel)                 (std::ostringstream << "467 " << channel << " :Channel key already set").str()
#define ERR_CHANNELISFULL(channel)          (std::ostringstream << "471 " << channel << " :Cannot join channel (+l)").str()
#define ERR_UNKNOWNMODE(chara)              (std::ostringstream << "472 " << chara << " :is unknown mode char to me").str()
#define ERR_INVITEONLYCHAN(channel)         (std::ostringstream << "473 " << channel << " :Cannot join channel (+i)").str()
#define ERR_BADCHANNELKEY(channel)          (std::ostringstream << "475 " << channel << " :Cannot join channel (+k)").str()

// general
#define ERR_NOSUCHCHANNEL(channel)          (std::ostringstream << "403 " << channel << " :No such channel").str()
#define ERR_CANNOTSENDTOCHAN(channel)       (std::ostringstream << "404 " << channel << " :Cannot send to channel").str()
#define ERR_NORECIPIENT(command)            (std::ostringstream << "411 :No recipient given " << command).str()
#define ERR_NOTEXTTOSEND                    std::string("412 :No text to send")
#define ERR_NOTONCHANNEL(channel)           (std::ostringstream << "442 " << channel << " :You're not on that channel").str()
#define ERR_NEEDMOREPARAMS(command)         (std::ostringstream << "461 " << command << " :You may not reregister").str()
#define ERR_CHANOPRIVSNEEDED(channel)       (std::ostringstream << "482 " << channel << " :You're not channel operator").str()

// MOTD
#define RPL_MOTDSTART(msg)  (std::ostringstream << "375 :- " << msg).str()
#define RPL_MOTD(msg)       (std::ostringstream << "372 :- " << msg).str()
#define RPL_ENDOFMOTD(msg)  (std::ostringstream << "376 :End of /MOTD command").str()

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
