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
