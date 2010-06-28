
#ifndef WFAPITESTCLIENT_PAN_H
#define WFAPITESTCLIENT_PAN_H

/** WFAPITestClient application panic codes */
enum TWFAPITestClientPanics
	{
	EWFAPITestClientUi = 1
	// add further panics here
	};

inline void Panic(TWFAPITestClientPanics aReason)
	{
	_LIT(applicationName,"WFAPITestClient");
	User::Panic(applicationName, aReason);
	}

#endif // WFAPITESTCLIENT_PAN_H
