import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.List;

public interface PolygonalChainProcessor extends Remote {

	/**
	 * Metoda pozwala na poznanie limitu współbieżnych konwersji, które może
	 * realizować serwis. Limit nie ulega zmianie w trakcie pracy serwisu.
	 * 
	 * @return limit współbieżnych zadań przetwarzania linii łamanych.
	 * @throws RemoteException wyjątek sygnalizujący wystąpinie błędu
	 */
	public int getConcurrentTasksLimit() throws RemoteException;

	/**
	 * Metoda dokonuje przetworzenia linii łamanej. Wynikiem jest pojedyncza liczba
	 * typu całkowitego.
	 * 
	 * @param name           nazwa linii łamanej
	 * @param polygonalChain lista istotnych punktów należących do linii łamanej.
	 *                       Pierwszy punkt na liście to początek linii, ostatni to
	 *                       koniec linii, punkty pośrednie to początki kolejnych
	 *                       odcinków prowadzących od początku linii po jej koniec.
	 *                       Uwaga: punkt pierwszy jest jednocześnie początkiem
	 *                       linii łamanej i pierwszego odcinka - nie powinien on
	 *                       zostać powielony.
	 * @return wynik przetworzenia linii łamanej.
	 * @throws RemoteException sygnalizacja błędu
	 */
	public int process(String name, List<Position2D> polygonalChain) throws RemoteException;

}
