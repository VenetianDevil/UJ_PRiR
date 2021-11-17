import java.util.Arrays;
import java.util.Collections;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.concurrent.atomic.AtomicReference;

public class Board implements BoardInterface {
	////////////////////////////////////////
	static final int N = 17; // board size
	static final int center_x = -1; // random if out of board
	static final int center_y = -1; // random if out of board
	static final int pawns = 0; // random if 0
	static final int max_delay
		= 500; // 0 disables, negative delay will add some randomness
	static final int print_delay
		= 50; // 0 disables, will suspend before print and resume after
	////////////////////////////////////////
	class Pawn implements PawnInterface {
		private final int m_id;
		private final AtomicReference<Thread> m_my_thread
			= new AtomicReference<Thread>();
		private final Coordinates m_coords = new Coordinates();

		public Pawn(int x, int y)
		{
			m_id = m_id_generator.incrementAndGet();
			m_coords.x = x;
			m_coords.y = y;
		}

		private void delay(int time)
		{
			try {
				Thread.sleep(time);
			} catch (InterruptedException e) {
			}
		}

		public int getID()
		{
			return m_id;
		}
		public void registerThread(Thread thread)
		{
			m_my_thread.set(thread);
		}

		private void check()
		{
			if (Thread.currentThread() != m_my_thread.get()) {
				//throw new RuntimeException("Pionek poruszony złym pionkiem");
				System.err.println("Pionek puruszony złym pionkiem");
				System.exit(0);
			}
			if (!m_running.get()) {
//				throw new RuntimeException(
//						"Pionek nie powinien się teraz przesuwać");
				System.err.println("Pionek nie powinien się teraz przesuwać");
				System.exit(0);

			}
		}
		private void makeMove(Move move)
		{
			check();
			// System.out.println("Poruszamy " + m_id + " " + move);
			final int new_x = m_coords.x + move.x;
			final int new_y = m_coords.y + move.y;
			final int old_x = m_coords.x;
			final int old_y = m_coords.y;

			if (!m_array.compareAndSet(index(new_x, new_y), 0, m_id))
				throw new RuntimeException(
					"Pole (" + new_x + ", " + new_y + ") nie jest puste");
			//
			m_map.put(new Coordinates(new_x, new_y), this);
			m_coords.x = new_x;
			m_coords.y = new_y;
			if (max_delay > 0) {
				delay(max_delay);
			} else if (max_delay < 0) {
				delay(
					-max_delay / 2
					+ ThreadLocalRandom.current().nextInt(-max_delay));
			}
			m_map.remove(new Coordinates(old_x, old_y));

			if (!m_array.compareAndSet(index(old_x, old_y), m_id, 0))
				throw new RuntimeException(
					"Pole (" + old_x + ", " + old_y
					+ ") zostało zajęte przez kogoś innego");
		}
		public int moveLeft()
		{
			makeMove(Move.LEFT);
			return m_coords.y;
		}
		public int moveRight()
		{
			makeMove(Move.RIGHT);
			return m_coords.y;
		}
		public int moveUp()
		{
			makeMove(Move.UP);
			return m_coords.x;
		}
		public int moveDown()
		{
			makeMove(Move.DOWN);
			return m_coords.x;
		}
	}

	private final int m_size;
	private final int m_middlepoint_row;
	private final int m_middlepoint_col;
	private final AtomicBoolean m_running = new AtomicBoolean(false);
	private final AtomicIntegerArray m_array;
	private final CountDownLatch m_start_signal = new CountDownLatch(1);
	private final ConcurrentHashMap<Coordinates, Pawn> m_map
		= new ConcurrentHashMap<Coordinates, Pawn>();

	private final AtomicInteger m_id_generator = new AtomicInteger();

	public static void main(String[] args)
	{
		Thread.UncaughtExceptionHandler h
			= new Thread.UncaughtExceptionHandler() {
				  @Override
				  public void uncaughtException(Thread th, Throwable ex)
				  {
					  System.out.println("Uncaught exception: " + ex);
					  System.exit(1);
				  }
			  };

		OptimizerInterface optimizer = new Optimizer();
		Board board = new Board(
			N,
			(center_x >= 0 && center_x < N)
				? N
				: ThreadLocalRandom.current().nextInt(N),
			(center_y >= 0 && center_y < N)
				? N
				: ThreadLocalRandom.current().nextInt(N));

		{
			final Integer[] indices = new Integer[N * N];
			Arrays.setAll(indices, i -> i);
			Collections.shuffle(Arrays.asList(indices));
			final int limit = 0 < pawns && pawns < N * N
				? pawns
				: ThreadLocalRandom.current().nextInt(1, N * N);
			for (int i = 70; i > 0; --i) {
				final int x = indices[i];
				board.addPawn(x / N, x % N);
			}


		}
		//System.out.println("Zx " + board.getMeetingPointCol() + " Zy " + board.getMeetingPointRow());
		board.print();
		//System.out.println("test1");
		if (print_delay > 0) {
			Thread p = new Thread(() -> {
				board.waitForStart();
				while (board.m_running.get()) {
					try {
						Thread.sleep(print_delay);
					} catch (InterruptedException e) {
					}
					optimizer.suspend();
					board.print();
					optimizer.resume();
				}
			});
			p.setDaemon(true);
			p.start();
		}
		board.m_running.set(true);
		optimizer.setBoard(board);

//		try {
//			Thread.sleep(1000);
//			optimizer.suspend();
//			System.out.println("Wątki zostay zawieszone");
//			Thread.sleep(10000);
//			System.out.println("Wątki będą odwieszone");
//			optimizer.resume();
//		} catch (InterruptedException e) {
//			e.printStackTrace();
//		}


		board.waitUntillDone();
		try {
			Thread.sleep(50);
		} catch (InterruptedException e) {
		}
		board.print();
		board.print();
	}

	public Board(int N)
	{
		this(N, (int)Math.floor((N - 1) / 2));
	}
	public Board(int N, int middlepoint)
	{
		this(N, middlepoint, middlepoint);
	}
	public Board(int N, int middlepoint_row, int middlepoint_col)
	{
		System.out.println(
			"Board: " + N + ", (" + middlepoint_row + ", " + middlepoint_col
			+ ")");
		m_size = N;
		m_middlepoint_row = 14;//middlepoint_row;
		m_middlepoint_col = 3;//middlepoint_col;
		m_array = new AtomicIntegerArray(m_size * m_size);
	}

	public int getSize()
	{
		return m_size;
	}
	public int getMeetingPointCol()
	{
		return m_middlepoint_col;
	}
	public int getMeetingPointRow()
	{
		return m_middlepoint_row;
	}

	public void addPawn(int x, int y)
	{
		// System.out.println("Adding pawn on (" + x + ", " + y + ")");
		Pawn tmp = new Pawn(x, y);
		if (!m_array.compareAndSet(index(x, y), 0, tmp.getID())) {
			throw new RuntimeException(
				"Pionek już znajduje się na pozycji (" + x + ", " + y + ")");
		}
		m_map.put(new Coordinates(x, y), tmp);
	}

	public Optional<PawnInterface> get(int col, int row)
	{
		synchronized (m_array)
		{
			final int id = m_array.get(index(row, col));
			if (id == 0) {
				return Optional.empty();
			} else {
				return Optional.of(m_map.get(new Coordinates(row, col)));
			}
		}
	}

	private int index(int x, int y)
	{
		return m_size * x + y;
	}

	public void optimizationDone()
	{
		m_running.set(false);
		synchronized (m_running)
		{
			m_running.notifyAll();
		}
		System.out.println("Optimization done");
	}
	public void optimizationStart()
	{
		m_running.set(true);
		m_start_signal.countDown();
		System.out.println("Starting optimization");
	}
	public void waitForStart()
	{
		try {
			m_start_signal.await();
		} catch (InterruptedException e) {
		}
	}
	public void waitUntillDone()
	{
		synchronized (m_running)
		{
			while (m_running.get()) {
				try {
					m_running.wait();
				} catch (InterruptedException e) {
				}
			}
		}
	}

	public void print()
	{
		//return;
		String out = "";
		String linia = "[90m"
			+ String.join("-----", Collections.nCopies(m_size + 1, "+")) + "\n";
		for (int n_row = m_size - 1; n_row >= 0; --n_row) {
			out += linia;
			for (int n_col = 0; n_col < m_size; ++n_col) {
				int id = m_array.get(index(n_row, n_col));
				Pawn x;
				Thread t;
				if (id == 0)
					out += "[90m|     ";
				else if (
					(x = m_map.get(new Coordinates(n_row, n_col))) != null
					&& (t = x.m_my_thread.get()) != null
					&& t.getState() == Thread.State.TERMINATED)
					out += String.format(
						"[90m| [%dm%3d[0m ", 32 + id % 7, id);
				else
					out += String.format(
						"[90m|[7m[%dm*%3d*[0m", 32 + id % 7, id);
			}
			out += "|\n";
		}
		out += linia + "[0m";
		System.out.println(out);
	}
}


class Coordinates {
	public int x;
	public int y;

	public Coordinates(int new_x, int new_y)
	{
		x = new_x;
		y = new_y;
	}
	public Coordinates()
	{
		this(0, 0);
	}
	public Coordinates(Coordinates copy)
	{
		this(copy.x, copy.y);
	}

	public String toString()
	{
		return "(" + x + ", " + y + ")";
	}
	public Coordinates copy()
	{
		return new Coordinates(this);
	}
	public Coordinates add(Move m)
	{
		return new Coordinates(x + m.x, y + m.y);
	}

	public boolean equals(Object o)
	{
		if (o == this)
			return true;
		if (!(o instanceof Coordinates))
			return false;
		Coordinates other = (Coordinates)o;
		return (other.x == x && other.y == y);
	}
	public int hashCode()
	{
		return Objects.hash(x, y);
	}
}

enum Move {
	UP(1, 0),
	DOWN(-1, 0),
	RIGHT(0, 1),
	LEFT(0, -1),
    ;

	public final int x;
	public final int y;

	Move(int new_x, int new_y)
	{
		x = new_x;
		y = new_y;
	}
}
