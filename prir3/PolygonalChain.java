import java.rmi.Remote;
import java.rmi.RemoteException;

/**
 * Interfejs systemu obsługującego linie łamane.
 */
public interface PolygonalChain extends Remote {

	/**
	 * Metoda tworzy nową linię łamaną.
	 * 
	 * @param name       nazwa linii łamanej
	 * @param firstPoint pierwszy punkt należący do linii łamanej
	 * @param lastPoint  ostatni punkt należący do linii łamanej
	 * @throws RemoteException wyjątek zgłaszany w przypadku błędu
	 */
	public void newPolygonalChain(String name, Position2D firstPoint, Position2D lastPoint) throws RemoteException;

	/**
	 * Metoda dodaje odcinek do linii łamanej o podanej nazwie.
	 * 
	 * @param name       nazwa linii łąmanej
	 * @param firstPoint pierwszy punkt odcinka
	 * @param lastPoint  ostatni punkt odcinka
	 * @throws RemoteException wyjątek zgłaszany w przypadku błędu
	 */
	public void addLineSegment(String name, Position2D firstPoint, Position2D lastPoint) throws RemoteException;

	/**
	 * Metoda zwraca wynik otrzymany dla danej linii łamanej z serwisu
	 * przetwarzającego linie łamane.
	 * 
	 * @param name nazwa linii łamanej
	 * @return wynik uzyskany z serwisu przetwarzającego lub null jeśli linia nie
	 *         jest znana lub nie została jeszcze przetworzona
	 * @throws RemoteException wyjątek zgłaszany w przypadku wystąpienia błędu
	 */
	public Integer getResult(String name) throws RemoteException;

	/**
	 * Metoda przekazuje URI umożliwiając dostęp do serwisu przetwarzającego linie
	 * łamane.
	 * 
	 * @param uri adres serwisu
	 * @throws RemoteException wyjątek zgłaszany w przypadku wystąpienia błędu
	 */
	public void setPolygonalChainProcessorName(String uri) throws RemoteException;
}
