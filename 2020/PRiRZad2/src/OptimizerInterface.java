
public interface OptimizerInterface {
	
	/**
	 * Metoda umożliwia przekazanie planszy z pionkami
	 * 
	 * @param board plansza
	 */
	public void setBoard( BoardInterface board );
	
	/**
	 * Wstrzymanie pracy wątków. Po zakończeniu metody 
	 * żaden z pionków nie może być w tracie zmiany położenia.
	 */
	public void suspend();
	
	/**
	 * Kontynuacja pracy wątkow.
	 */
	public void resume();

}
