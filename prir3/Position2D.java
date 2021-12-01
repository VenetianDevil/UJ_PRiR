import java.io.Serializable;
import java.util.Objects;

/**
 * Klasa służąca do tworzenia obiektów niezmieniczych reprezentujących położenia
 * na płaszczyźnie. Położenia są liczbami całkowitymi.
 *
 */
public class Position2D implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1190336672323638590L;
	private final int col;
	private final int row;

	public Position2D(int col, int row) {
		this.col = col;
		this.row = row;
	}

	/**
	 * Numer wiersza
	 * 
	 * @return wiersz
	 */
	public int getRow() {
		return row;
	}

	/**
	 * Numer kolumny
	 * 
	 * @return kolumna
	 */
	public int getCol() {
		return col;
	}

	@Override
	public String toString() {
		return "Position2D [col=" + col + ", row=" + row + "]";
	}

	@Override
	public int hashCode() {
		return Objects.hash(col, row);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Position2D other = (Position2D) obj;
		return col == other.col && row == other.row;
	}
}
