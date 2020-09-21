#include "CRS232.h"
#include "LineParser.h"
#include <conio.h>
#include "SynchroEventListener.h"
#include <iostream>

int main(int argc, char * argv[])
{
	char test[] = "COM1";
	SynchroEventListener * listener = new SynchroEventListener();
	Parser * parser = new LineParser();
	parser->addSerialEventListener(listener);
	CRS232 * comm = new CRS232(test, 9600, 8, NOPARITY, ONESTOPBIT, parser);

	while (1)
	{
		while (listener->getMessages().Size() > 0)
		{
			Message * msg = listener->getMessages().Get();
			std::cout << "Ligne reçue : ";
			for (int i = 0; i < msg->getData().size(); i++)
				std::cout << msg->getData()[i];
			std::cout << std::endl;
			delete msg;
		}

		comm->sendBuf((char*)"Test\n", 5);
	}

	
	return 0;
}