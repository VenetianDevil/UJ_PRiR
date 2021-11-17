import java.util.Optional;

public interface BoardInterface {
	/**
	 * Rozmiar planszy. Dozwolone indeksy od 0 do size-1.
	 * @return rozmiar planszy.
	 */
	public int getSize();
	
	/**
	 * Metoda zwraca informację o pionku znajdującym się na pozycji col/row.
	 * @param col kolumna
	 * @param row wiersz
	 * @return Informacja o pionku. Brak pionka oznacza pusty obiekt Optional.
	 */
	public Optional<PawnInterface> get( int col, int row );
	
	/**
	 * Metoda zwraca kolumnę, w której znajduje się punkt zborny.
	 * @return indeks kolumny punktu zbornego.
	 */
	public int getMeetingPointCol();

	/**
	 * Metoda zwraca wiersz, w którym znajduje się punkt zborny.
	 * @return indeks wiersza punktu zbornego.
	 */
	public int getMeetingPointRow();
	
	/**
	 * Metoda wywoływana po zakończeniu procesu optymalizacji położeń 
	 * pionków.
	 */
	public void optimizationDone();
	
	/**
	 * Metoda informuje o rozpoczęciu optymalizacji położeń
	 */
	public void optimizationStart();
}
