#ifndef __ConnectionListener_h__
#define __ConnectionListener_h__

class ConnectionListener {
public:
	virtual void onMessage(std::string message) {};
	virtual void onClose(std::string reason) {};
};

#endif