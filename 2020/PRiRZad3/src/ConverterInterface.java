import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.List;

public interface ConverterInterface extends Remote {
	/**
	 * Metoda konwertuje ciąg liczb dostarczonych jako "input".
	 * 
	 * @param input ciąg liczb do skonwertowania
	 * @return ciąg po wykonaniu konwersji
	 * @throws RemoteException wyjątek wymagany przez RMI
	 */
	public List<Integer> convert(List<Integer> input) throws RemoteException;
}
