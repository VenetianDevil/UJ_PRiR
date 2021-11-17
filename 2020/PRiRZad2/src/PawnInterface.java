
public interface PawnInterface {
	/**
	 * Metoda zwraca identyfikator pionka
	 * 
	 * @return unikalny identyfikator pionka
	 */
	public int getID();

	/**
	 * Zlecenie przesunięcia pionka w lewo (dekrementacja indeksu kolumny)
	 * Metoda blokuje wątek, który ją wykonał na czas realizacji
	 * zleconej operacji.
	 * 
	 * @return metoda zwraca indeks kolumny, w której ruch się zakończył.
	 */
	public int moveLeft();

	/**
	 * Zlecenie przesunięcia pionka w prawo (inkrementacja indeksu kolumny)
	 * Metoda blokuje wątek, który ją wykonał na czas realizacji
	 * zleconej operacji.
	 * 
	 * @return metoda zwraca indeks kolumny, w której ruch się zakończył.
	 */
	public int moveRight();

	/**
	 * Zlecenie przesunięcia pionka w górę (inkrementacja indeksu wiersza).
	 * Metoda blokuje wątek, który ją wykonał na czas realizacji
	 * zleconej operacji.
	 * 
	 * @return metoda zwraca indeks wiersza, w której ruch się zakończył.
	 */
	public int moveUp();

	/**
	 * Zlecenie przesunięcia pionka w dół (dekrementacja indeksu wiersza).
	 * Metoda blokuje wątek, który ją wykonał na czas realizacji
	 * zleconej operacji.
	 * 
	 * @return metoda zwraca indeks wiersza, w której ruch się zakończył.
	 */
	public int moveDown();

	/**
	 * Rejestracja wątku, który będzie wykonywał operacje na tym pionku. Rejestracja
	 * musi zostać dokonana przed pierwszym zleceniem ruchu pionka.
	 * 
	 * @param thread wątek odpowiedzialny za ruch pionka.
	 */
	public void registerThread(Thread thread);
}
