# ULClient

For questions find us in Discord: https://discord.gg/yuDngyj

ULClient can be built using Visual Studio 2017. You'll need to download several updates after downloading MSVS 2017 Community. 
Specifically you need to make sure you have the v140_xp Windows SDK. You can download all the necessary SDKs from Microsoft's website.

You should also set the startup project to Underlight - not AgentSvr or AgentSvc. 

You'll need the other Underlight assets in the target directory you're running the executable from (by default: C:/lyra/dev). Those assets
can be found in this repository, under assets/


before building search and change the changeme.localhost to your server's IP.
