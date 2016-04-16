#ifndef CLOCK_H_
#define CLOCK_H_


class Clock
{
	public:

	Clock();

	//Startuje odliczanie zegara
	void start();

	//Zatrzymuje odliczanie zegara zegar
	void stop();

	//Restartuje zegar - przywraca wartosc zerowa czasu
	void restart();

	//Zwraca czas w [ms] od ostatniego startu/restartu
	unsigned long get_ms();


	private:


};

extern bool respond;
extern Clock *clock;

#endif