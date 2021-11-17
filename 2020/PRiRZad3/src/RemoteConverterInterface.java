import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.List;

public interface RemoteConverterInterface extends Remote {
	/**
	 * Rejestracja użytkownika.
	 * 
	 * @return unikalny identyfikator użytkownika systemu
	 * @throws RemoteException
	 */
	public int registerUser() throws RemoteException;

	/**
	 * Zlecenie dodania wartości value do listy danych dla użytkownika userID.
	 * 
	 * @param userID numer użytkownika, który dodaje daną
	 * @param value  dana do dodania
	 * @throws RemoteException
	 */
	public void addDataToList(int userID, int value) throws RemoteException;

	/**
	 * Metoda pozwala na przekazanie informacji o URL, pod którym można zlokalizować
	 * serwis dokonujący konwersji.
	 * 
	 * @param url adres serwisu dokonującego konwersji
	 * @throws RemoteException
	 */
	public void setConverterURL(String url) throws RemoteException;

	/**
	 * Koniec danych przekazywanych przez użytkownika o podanym userID. Przekazane
	 * dane należy przekazać do konwersji.
	 * 
	 * @param userID identyfikator użytkownika
	 * @throws RemoteException
	 */
	public void endOfData(int userID) throws RemoteException;

	/**
	 * Metoda pozwalająca na sprawdzenie czy proces obsługi danych dla użytkownika
	 * userID został zakończony. Przez proces obsługi rozumie się przekazanie przez
	 * użytkownika danych, zakończnie przekazywania danych poprzez wywołanie
	 * <code>endOfData</code> i wykonanie konwersji.
	 * 
	 * @param userID identyfikator użytkownika
	 * @return true - zakończono przetwarzanie danych dla userID, false - dane w
	 *         trakcie przetwarzania.
	 * @throws RemoteException
	 */
	public boolean resultReady(int userID) throws RemoteException;

	/**
	 * Wynik przetwarzania danych dla użytkownika userID. Zwracana lista to dane
	 * przekazane przez użytkownika i poddane konwersji.
	 * 
	 * @param userID identyfikator użytkownika
	 * @return wynik przetwarzania danych
	 * @throws RemoteException
	 */
	public List<Integer> getResult(int userID) throws RemoteException;
}
