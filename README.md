bot2learn
=========

A simple IRC bot(net) created for myself out of enjoyment and trying to become a better programmer. Should give me enough practice on normal and RAW socket programming, translating RFCs to code, Object-Oriented programming, C/C++ and using GIT to fill a few lost nights the coming year!

Although botnets are commonly accociated with malicious intent, I do not condone or want to promote this is any way. Everything written here is purely for my own education. 

I'd like to try and build the various components as modular as I can by using OOB so I can reuse pieces of it in other projects. some of the modules I'd like to come out of this little hobby are:

- Generic socket implementation. Handling send & receive buffers etc
- RAW socket class allowing to easily generate custom packets
- Basic IRC client communication library based on RFCs 1459 and 2812 
- Basic 'worker' bot derived from the IRC client class
- 'Master' bot derived from the IRC client class (or maybe the worker bot?!) able to relay commands etc to the worker bots
- Anything else I might come up with along the way


About the Author:

In my professional life I work in the networking field and am not a proficient programmer at all. I do like to dabble in programming just because it's fun and feel it supports my general computer knowledge and troubleshooting skills. The code written here is probably written very badly and I'd  recommend not using it in any of your own projects. Feel free to comment on it in any way though as I'd like to learn!


Random Ramblings from a Rambling man:

Initial code was based on this article: http://www.the-tech-tutorial.com/?p=1716
Eventually found a better code example here: http://jakash3.wordpress.com/2010/12/31/simple-c-irc-bot-for-linux/

Compile with:
g++ runBot.cpp bot.cpp sock.cpp cfgfile.cpp irc.cpp mytime.cpp -o runBot


Busy with creating a raw socket class that makes it easy to send single crafted packets like the python scapy library. do the following tcpdump to check packets (-vvv for extra verbose)

tcpdump -i lo port 6667 -s0 -nn


File explanations:
irc.h/cpp defines basic irc client functions like conneting to a server, parsing and sending messages.
bot.h/cpp is a subclass of class irc and enables the parsing of specific bot commands etc
sock.h/cpp is a basic socket handling class taken from an online tutorial with some minor edits by myself. 
rawsock.h/cpp is a basic raw socket handling lib taken from the covert client on sock-raw.org. All credits go out to them!
runBot.cpp is the main loop that reads in the config file and initialises the bot
mybot.cfg is the text-based configuration file. Currenly only containing the IRC servers host/port botnick and IRC channel
cfgfile.h/cpp is the libary used for handling the text based configuration file. This was taken from another online tutorial with some minor edits
mytime.h/cpp is a very small library containing a function to get the current date/time. Should probably move to a different library eventually for screen/logfile output handling

