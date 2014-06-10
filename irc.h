#ifndef IRC_H_
#define IRC_H_

#include <string>
#include "sock.h"

#define MAXDATASIZE 513                         // IRC RFC1459 states IRC messages shall not exceed 512 characters, counting all chars including trailing CR-LF


// Client-server command responses 
#define RPL_WELCOME "001"                       // Welcome to the Internet Relay Network <nick>!<user>@<host>
#define RPL_YOURHOST "002"                      // Your host is <servername>, running version <ver>
#define RPL_CREATED "003"                       // This server was created <date>
#define RPL_MYINFO "004"                        // <servername> <version> <available user modes> <available channel modes>
#define RPL_BOUNCE "005"                        // "Try server <server name>, port <port number>"

// server replies 
#define RPL_USERHOST "302"
#define RPL_ISON "303"


// AWAY messages
#define RPL_AWAY "301"                          // <nick> :<away message>
#define RPL_UNAWAY "305"                        // :You are no longer marked as being away
#define RPL_NOWAWAY "306"                       // :You have been marked as being away

// WHOIS messages
#define RPL_WHOISUSER "311"                     // <nick> <user> <host> * :<real name>
#define RPL_WHOISSERVER "312"                   // <nick> <server> :<server info>
#define RPL_WHOISOPERATOR "313"                 // <nick> :is an IRC operator
#define RPL_WHOISIDLE "317"                     // <nick> <integer> :seconds idle
#define RPL_ENDOFWHOIS "318"                    // <nick> :End of WHOIS list
#define RPL_WHOISCHANNELS "319"                 // <nick> :*9 ( "@" / "+" ) <channel> " " )
#define RPL_WHOWASUSER "314"                    // <nick> <user> <host> * :<real name>
#define RPL_ENDOFWHOWAS "369"                   // <nick> :End of WHOWAS

// LIST messages
#define RPL_LIST "322"                          // <channel> <# visible> :<topic>
#define RPL_LISTEND "323"                       // :End of LIST

// MOTD messages
#define RPL_MOTDSTART "375"                     // :- <server> Message of the day - "
#define RPL_MOTD "372"                          // :- <text>
#define RPL_ENDOFMOTD "376"                     // :End of MOTD command
#define ERR_NOMOTD "422"

// NAMES messages
#define RPL_ENDOFNAMES "366"                    // <channel> :End of NAMES list
// NICK messages
#define ERR_NICKNAMEINUSE "433"                 // <nick> :Nickname is already in use


class IrcClient
{
  public:
    IrcClient();
    virtual ~IrcClient();

    bool setup;

    void connect(const char *_host, const char *_port, const char *_nick, const char *_channel);
    int start();


    const char *port;
    const char *host;
    const char *nick;
    const char *channel;

    plug sockfd;



  private:
//    const char *port;
//    const char *host;
//    const char *nick;
//    const char *channel;

 //   plug sockfd;                        // The socket descriptor
}; 


class IrcMessage
{
  public:
    IrcMessage();

    std::string prefix;			// This is the full (optional) prefix which is the source of the received message 
    std::string prefixNick;		// This is the extracted nickname of the prefix string
    std::string prefixUser;		// The extracted user from the prefix string
    std::string prefixHost;		// The extracted host from the prefix string
    std::string command;		// This is the used command
    std::string parameters;		// This contains all the message parameters
    std::string msgtarget;		// In case of a PRIVMSG, this variable holds the target of the message

    void recv(std::string buf);				// This function parses a message from the IRC server and sets the class accordingly
    void send(plug sockfd, std::string cmd, std::string pars);	// This function sends a message to the IRC server and prints it to the terminal

  private:
};

class Irc
{
  public:
    Irc();
    ~Irc();
  
  private:
};

#endif /* IRC_H_ */
