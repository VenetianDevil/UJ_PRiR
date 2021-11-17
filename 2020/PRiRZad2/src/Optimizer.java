import java.util.*;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Phaser;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Optimizer implements OptimizerInterface  {
    BoardInterface board;

    //private static final ExecutorService executor = Executors.newSingleThreadExecutor();
    private MainExecutor mainExecutor;

    @Override
    public void setBoard(BoardInterface board) {
        this.board = board;



        List<Thread> allPawnThreads = new ArrayList<>();

        this.mainExecutor = new MainExecutor(board);

        for(int row = 0; row < this.board.getSize(); row++)
        {
            for(int col = 0; col < this.board.getSize(); col++)
            {
                if(this.board.get(col, row).isPresent())
                {
                    Optional<PawnInterface> pawn = this.board.get(col, row);
                    Runnable runnable = new MainPawnRunnable(pawn.get(), col, row, mainExecutor);
                    Thread thread = new Thread(runnable);
                    pawn.get().registerThread(thread);
                    allPawnThreads.add(thread);
                    System.out.println("rese" + this.board.get(col, row).get().getID());
                    mainExecutor.setPawnData(col, row, pawn.get());
                    mainExecutor.registerPawnToSuspendPhaser();
                }
            }
        }
        this.board.optimizationStart();
        allPawnThreads.forEach(Thread::start);

        new Thread(joinAllThreadAndRunOptimizationDone(allPawnThreads)).start();
    }

    private Runnable joinAllThreadAndRunOptimizationDone(List<Thread> allPawnThreads) {
        return () -> {
            allPawnThreads.forEach(thread -> {
                try {
                    thread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            });
            this.board.optimizationDone();
            //System.out.println("End");
        };
    }

    @Override
    public void suspend() {
        mainExecutor.suspend();
    }

    @Override
    public void resume() {
        mainExecutor.resume();
    }

    public static class MainPawnRunnable implements Runnable {

        private final PawnInterface pawn;
        private int row;
        private int col;
        private MainExecutor mainExecutor;

        public MainPawnRunnable(PawnInterface pawn, int col, int row, MainExecutor mainExecutor) {
            this.pawn = pawn;
            this.row = row;
            this.col = col;
            this.mainExecutor = mainExecutor;
        }

        @Override
        public void run() {
            //sprawdzić akytwność w warunku
            while (!mainExecutor.isEnd(pawn)) {
                System.out.println("Kolejny task " + pawn.getID());
                if( mainExecutor.setOccupiedDieIfPossible(pawn))
                    break;
                mainExecutor.createRunnable(pawn).run();
                mainExecutor.setOccupiedDieIfPossible(pawn);
            }
        }
    }

    private static final String LEFT_WAY = "left";
    private static final String RIGHT_WAY = "right";
    private static final String TOP_WAY = "top";
    private static final String DOWN_WAY = "down";

    public static class MakeMoveRunnable implements Runnable {
        private final MainExecutor mainExecutor;
        private PawnInterface pawn;
        private String wayToMove;
        private int col;
        private int row;
        private int newCol;
        private int newRow;

        public MakeMoveRunnable(PawnInterface pawn, String wayToMove, MainExecutor mainExecutor, int col, int row) {
            this.pawn = pawn;
            this.wayToMove = wayToMove;
            this.mainExecutor = mainExecutor;
            this.col = col;
            this.row = row;
        }

        @Override
        public void run() {
            if (wayToMove.equals(LEFT_WAY)) {
                this.newCol = pawn.moveLeft();
                this.newRow = row;
            }

            if(wayToMove.equals(RIGHT_WAY))
            {
                this.newCol = pawn.moveRight();
                this.newRow = row;
            }

            if(wayToMove.equals(TOP_WAY))
            {
                this.newRow = pawn.moveUp();
                this.newCol = col;
            }

            if(wayToMove.equals(DOWN_WAY))
            {
                this.newRow = pawn.moveDown();
                this.newCol = col;
            }

            this.mainExecutor.modifyOccupiedStructure(pawn, col, row, newCol, newRow);
        }
    }

    public static class MakeDieRunnable implements Runnable{
        @Override
        public void run() {
            System.out.println("Tak");
        }
    }

    public static class MakeWaitRunnable implements Runnable {

        private PawnInterface pawn;
        private int waitPawnId;
        private MainExecutor mainExecutor;

        public MakeWaitRunnable(PawnInterface pawn, int waitPawnId, MainExecutor mainExecutor) {
            this.pawn = pawn;
            this.waitPawnId = waitPawnId;
            this.mainExecutor = mainExecutor;
        }

        @Override
        public void run() {
            mainExecutor.waitForPawn(pawn, waitPawnId);
        }
    }

    public static class MakeSuspendRunnable implements Runnable {

        private MainExecutor mainExecutor;
        private PawnInterface pawn;

        public MakeSuspendRunnable(MainExecutor mainExecutor, PawnInterface pawn) {
            this.mainExecutor = mainExecutor;
            this.pawn = pawn;
        }

        @Override
        public void run() {
            mainExecutor.suspendPawn(pawn);
        }
    }

    public static class PawnLocation {

        private int col;
        private int row;

        public PawnLocation(int col, int row) {
            this.col = col;
            this.row = row;
        }

        public int getCol() {
            return col;
        }

        public int getRow() {
            return row;
        }
    }

    //ReentrantLock, CyclclicBarrier (może się przyda CountDownLatch), volatile (możesz poczytać)
    //System.out.println(Thread.currentThread().getId())
    public static class MainExecutor {

        //CyclicBarrier cb = new CyclicBarrier(2);
        private static final String OCCUPIED = "OCCUPIED";
        private static final String WILL_BE_OCCUPIED = "WILL_BE_OCCUPIED";
        private static final String DIE = "DIE";
        private Map<String,Map<String, PawnInterface>> occupiedMap;
        private Map<Integer, PawnLocation> locationsById = new HashMap<>();
        private Map<Integer, Phaser> phasersById = new HashMap<>();

        private BoardInterface board;
        private Lock lock = new ReentrantLock();

        private AtomicBoolean running = new AtomicBoolean(true);
        private Phaser suspendPhaser = new Phaser(1);

        public MainExecutor(BoardInterface board) {
            this.board = board;
            occupiedMap = new HashMap<>();
            occupiedMap.put(OCCUPIED, new HashMap<>());
            occupiedMap.put(WILL_BE_OCCUPIED, new HashMap<>());
            occupiedMap.put(DIE, new HashMap<>());
        }

        public boolean setOccupiedDieIfPossible(PawnInterface pawn)
        {
            lock.lock();
            try {

                int col = locationsById.get(pawn.getID()).getCol();
                int row = locationsById.get(pawn.getID()).getRow();
                System.out.println("Testuje col:" + col + " row:"+row);
                if (col == board.getMeetingPointCol() && row == board.getMeetingPointRow()) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (col == board.getMeetingPointCol() && occupiedMap.get(DIE).containsKey(createLocation(col, row + 1))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (col == board.getMeetingPointCol() && occupiedMap.get(DIE).containsKey(createLocation(col, row - 1))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (row == board.getMeetingPointRow() && occupiedMap.get(DIE).containsKey(createLocation(col + 1, row))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (row == board.getMeetingPointRow() && occupiedMap.get(DIE).containsKey(createLocation(col - 1, row))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (row < board.getMeetingPointRow() && col < board.getMeetingPointCol() &&
                        occupiedMap.get(DIE).containsKey(createLocation(col + 1, row)) &&
                        occupiedMap.get(DIE).containsKey(createLocation(col, row + 1))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (row > board.getMeetingPointRow() && col < board.getMeetingPointCol() &&
                        occupiedMap.get(DIE).containsKey(createLocation(col + 1, row)) &&
                        occupiedMap.get(DIE).containsKey(createLocation(col, row - 1))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (row > board.getMeetingPointRow() && col > board.getMeetingPointCol() &&
                        occupiedMap.get(DIE).containsKey(createLocation(col - 1, row)) &&
                        occupiedMap.get(DIE).containsKey(createLocation(col, row - 1))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                } else if (row < board.getMeetingPointRow() && col > board.getMeetingPointCol() &&
                        occupiedMap.get(DIE).containsKey(createLocation(col - 1, row)) &&
                        occupiedMap.get(DIE).containsKey(createLocation(col, row + 1))) {
                    setOccupiedDie(pawn, col, row);
                    return true;
                }
                System.out.println("NIe znalazlo");
                return false;
            } finally {
                lock.unlock();
            }
        }

        public boolean isEnd(PawnInterface pawn) {
            lock.lock();
            try {
                int col = locationsById.get(pawn.getID()).getCol();
                int row = locationsById.get(pawn.getID()).getRow();

                return occupiedMap.get(DIE).containsKey(createLocation(col, row));
            } finally {
                lock.unlock();
            }
        }

        private void setOccupiedDie(PawnInterface pawn, int col, int row)
        {
            lock.lock();
            try {
                phasersById.get(pawn.getID()).arriveAndAwaitAdvance();
                suspendPhaser.arriveAndDeregister();
                System.out.println("UStawionon DIE! pid:" + pawn.getID() + " cor col:" + col + " row:" + row);
                occupiedMap.get(DIE).put(createLocation(col, row), pawn);
                if(occupiedMap.get(OCCUPIED).containsKey(createLocation(col, row)))
                {
                    occupiedMap.get(OCCUPIED).remove(createLocation(col, row));
                }

                if(occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col, row)))
                {
                    occupiedMap.get(WILL_BE_OCCUPIED).remove(createLocation(col, row));
                }
                phasersById.get(pawn.getID()).arriveAndDeregister();
            } finally {
                lock.unlock();
            }

        }

        public void modifyOccupiedStructure(PawnInterface pawn, int col, int row, int newCol, int newRow)
        {
            lock.lock();
            try
            {
                occupiedMap.get(OCCUPIED).remove(createLocation(col, row));
                if(col != newCol)
                {
                    occupiedMap.get(WILL_BE_OCCUPIED).remove(createLocation(newCol, row));
                    occupiedMap.get(OCCUPIED).put(createLocation(newCol, row), pawn);
                } else
                {
                    occupiedMap.get(WILL_BE_OCCUPIED).remove(createLocation(col, newRow));
                    occupiedMap.get(OCCUPIED).put(createLocation(col, newRow), pawn);
                }
                locationsById.put(pawn.getID(), new PawnLocation(newCol, newRow));
                System.out.println("Rozpoczęto wykonywanie główny pionek pid:" + pawn.getID());
                phasersById.get(pawn.getID()).arriveAndAwaitAdvance();
                System.out.println("Zakończono wykonywanie główny pionek pid:" + pawn.getID());
            } finally {
                lock.unlock();
            }
        }

        public void setPawnData(int col, int row, PawnInterface pawn) {
            lock.lock();
            try {
                occupiedMap.get(OCCUPIED).put(createLocation(col, row), pawn);
                locationsById.put(pawn.getID(), new PawnLocation(col, row));
                phasersById.put(pawn.getID(), new Phaser(1));
            } finally {
                lock.unlock();
            }
        }

        public void putWillBeOccupiedPosition(int col, int row, PawnInterface pawn)
        {
            lock.lock();
            try {
                occupiedMap.get(WILL_BE_OCCUPIED).put(createLocation(col, row), pawn);
            } finally {
                lock.unlock();
            }
        }

        public Runnable createRunnable(PawnInterface pawn) {

            if (!this.running.get()) {
                return new MakeSuspendRunnable(this, pawn);
            }

            lock.lock();

            int col = locationsById.get(pawn.getID()).getCol();
            int row = locationsById.get(pawn.getID()).getRow();

            try {

                if(col == board.getMeetingPointCol() && row == board.getMeetingPointRow())
                {
                    return ()->{};
                }

                if (row < board.getMeetingPointRow()) {
                    if (!(occupiedMap.get(OCCUPIED).containsKey(createLocation(col, row + 1)) ||
                            occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col, row + 1)) ||
                            occupiedMap.get(DIE).containsKey(createLocation(col, row + 1)))) {
                        //czy nie trzeba tu zrobic loca?
                        putWillBeOccupiedPosition(col, row + 1, pawn);
                        return new MakeMoveRunnable(pawn, TOP_WAY, this, col, row);
                    } else {

                        if(occupiedMap.get(OCCUPIED).containsKey(createLocation(col, row + 1))
                                || occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col, row + 1)))
                        {
                            System.out.println("Nienienie pid:" + pawn.getID() + " col:" + col + " row:" + row);


                        PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col, row + 1));
                        if (occupiedPawn == null) {
                            occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col, row + 1));
                        }


                        if(occupiedPawn != null) {
                            System.out.println("W occu lub will pid:" + pawn.getID());
                            return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                        }
                    }

                        if(occupiedMap.get(DIE).containsKey(createLocation(col, row + 1)))
                        {
                            if(col < board.getMeetingPointCol() && !occupiedMap.get(DIE).containsKey(createLocation(col + 1, row)))
                            {
                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col + 1, row));

                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col + 1, row));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col + 1, row, pawn);
                                return new MakeMoveRunnable(pawn, RIGHT_WAY, this, col, row);
                            }

                            if(col > board.getMeetingPointCol() && !occupiedMap.get(DIE).containsKey(createLocation(col - 1, row)))
                            {

                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col - 1, row));


                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col - 1, row));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col - 1, row, pawn);
                                return new MakeMoveRunnable(pawn, LEFT_WAY, this, col, row);
                            }
                        }

                        return new MakeDieRunnable();
                    }
                }

                if (col < board.getMeetingPointCol() ) {
                    if (!(occupiedMap.get(OCCUPIED).containsKey(createLocation(col + 1, row)) ||
                            occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col + 1, row)) ||
                            occupiedMap.get(DIE).containsKey(createLocation(col + 1, row)))) {
                        putWillBeOccupiedPosition(col + 1, row, pawn);
                        return new MakeMoveRunnable(pawn, RIGHT_WAY, this, col, row);
                    } else {

                        if(occupiedMap.get(OCCUPIED).containsKey(createLocation(col + 1, row))
                                || occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col + 1, row)))
                        {
                            System.out.println("Nienienie pid:" + pawn.getID() + " col:" + col + " row:" + row);


                        PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col + 1, row));
                        if (occupiedPawn == null) {
                            occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col + 1, row));
                        }


                        if(occupiedPawn != null) {
                            System.out.println("W occu lub will pid:" + pawn.getID());
                            return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                        }
                        }


                        if(occupiedMap.get(DIE).containsKey(createLocation(col + 1, row)))
                        {
                            System.out.println("W die dla pid:" + pawn.getID());
                            if(row < board.getMeetingPointRow() && !occupiedMap.get(DIE).containsKey(createLocation(col, row + 1)))
                            {

                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col, row + 1));

                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col, row + 1));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col, row + 1, pawn);
                                return new MakeMoveRunnable(pawn, TOP_WAY, this, col, row);
                            }

                            if(row > board.getMeetingPointRow() && !occupiedMap.get(DIE).containsKey(createLocation(col, row - 1)))
                            {
                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col, row - 1));

                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col, row - 1));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col, row - 1, pawn);
                                return new MakeMoveRunnable(pawn, DOWN_WAY, this, col, row);
                            }
                        }

                        return new MakeDieRunnable();
                    }
                }

                if (col > board.getMeetingPointCol()) {
                    if (!(occupiedMap.get(OCCUPIED).containsKey(createLocation(col - 1, row)) ||
                            occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col - 1, row)) ||
                            occupiedMap.get(DIE).containsKey(createLocation(col - 1, row)))) {
                        putWillBeOccupiedPosition(col - 1, row, pawn);
                        return new MakeMoveRunnable(pawn, LEFT_WAY, this, col, row);
                    } else {

                        if(occupiedMap.get(OCCUPIED).containsKey(createLocation(col - 1, row))
                                || occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col - 1, row)))
                        {
                            System.out.println("Nienienie pid:" + pawn.getID() + " col:" + col + " row:" + row);


                        PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col - 1, row));
                        if (occupiedPawn == null) {
                            occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col - 1, row));
                        }

                        if(occupiedPawn != null) {
                            System.out.println("W occu lub will pid:" + pawn.getID());
                            return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                        }
                        }

                        if(occupiedMap.get(DIE).containsKey(createLocation(col - 1, row )))
                        {
                            if(row < board.getMeetingPointRow() && !occupiedMap.get(DIE).containsKey(createLocation(col, row + 1)))
                            {
                                   PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col, row + 1));


                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col, row  + 1));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col, row + 1, pawn);
                                return new MakeMoveRunnable(pawn, TOP_WAY, this, col, row);
                            }

                            if(row > board.getMeetingPointRow() && !occupiedMap.get(DIE).containsKey(createLocation(col, row - 1)))
                            {
                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col, row - 1));

                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col, row - 1));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col, row - 1, pawn);
                                return new MakeMoveRunnable(pawn, DOWN_WAY, this, col, row);
                            }
                        }

                        return new MakeDieRunnable();
                    }
                }

                //lewa gorna ćwiartka
                if (row > board.getMeetingPointRow()) {
                    System.out.println("Pionek " + pawn.getID() + " wszedł to  row > ");
                    if (!(occupiedMap.get(OCCUPIED).containsKey(createLocation(col, row - 1)) ||
                            occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col, row - 1)) ||
                            occupiedMap.get(DIE).containsKey(createLocation(col, row - 1)))) {
                        System.out.println("Pionek " + pawn.getID() + "wedzl dalej row >");
                        putWillBeOccupiedPosition(col, row - 1, pawn);
                        return new MakeMoveRunnable(pawn, DOWN_WAY, this, col, row);
                    } else {

                        if(occupiedMap.get(OCCUPIED).containsKey(createLocation(col, row - 1))
                                || occupiedMap.get(WILL_BE_OCCUPIED).containsKey(createLocation(col, row - 1)))
                        {
                            System.out.println("Nienienie pid:" + pawn.getID() + " col:" + col + " row:" + row);


                        PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col, row - 1));
                        if (occupiedPawn == null) {
                            occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col, row - 1));
                        }

                        if(occupiedPawn != null) {
                            System.out.println("W occu lub will pid:" + pawn.getID());
                            return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                            }
                        }


                        if(occupiedMap.get(DIE).containsKey(createLocation(col, row - 1)))
                        {
                            if(col < board.getMeetingPointCol() && !occupiedMap.get(DIE).containsKey(createLocation(col + 1, row)))
                            {
                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col + 1, row));

                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col + 1, row));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col + 1, row, pawn);
                                return new MakeMoveRunnable(pawn, RIGHT_WAY, this, col, row);
                            }

                            if(col > board.getMeetingPointCol() && !occupiedMap.get(DIE).containsKey(createLocation(col - 1, row)))
                            {

                                PawnInterface occupiedPawn = occupiedMap.get(OCCUPIED).get(createLocation(col - 1, row));


                                if(occupiedPawn == null)
                                {
                                    occupiedPawn = occupiedMap.get(WILL_BE_OCCUPIED).get(createLocation(col - 1, row));
                                }

                                if(occupiedPawn != null) {
                                    System.out.println("W die occu lub will pid:" + pawn.getID());
                                    return new MakeWaitRunnable(pawn, occupiedPawn.getID(), this);
                                }

                                putWillBeOccupiedPosition(col - 1, row, pawn);
                                return new MakeMoveRunnable(pawn, LEFT_WAY, this, col, row);
                            }
                        }

                        return new MakeDieRunnable();
                    }
                }

            } finally {
                lock.unlock();
            }

            return () -> {
                System.out.println(pawn.getID());
                System.exit(0);
            };
            //throw new RuntimeException("To nie powinno się wydarzyć");//Do usunięcia
        }

        public void waitForPawn(PawnInterface pawn, int waitPawnId) {
            lock.lock();
            Phaser phaser = null;
            try {
                phaser = phasersById.get(waitPawnId);
            } finally {
                lock.unlock();
            }
            if (phaser != null) {
                //System.out.println("Rozpoczęto oczekiwanie! from:" + pawn.getID() + " pid:" + waitPawnId);
                phaser.register();
                System.out.println("Przed arrive and advance from:" + pawn.getID() + " pid:" + waitPawnId);
                phaser.arriveAndAwaitAdvance();
                System.out.println("Po arrive and advance from:" + pawn.getID() + " pid:" + waitPawnId);
                phaser.arriveAndDeregister();
                System.out.println("Zakończono oczekiwanie! from:" + pawn.getID() + " pid:" + waitPawnId);
            } else {
                throw new RuntimeException("Nie ma phasera dla id pionka.");
            }
        }

        public void suspend() {
            this.running.set(false);
            this.suspendPhaser.arriveAndAwaitAdvance();
        }

        public void resume() {
            this.running.set(true);
            this.suspendPhaser.arriveAndAwaitAdvance();
        }

        public void registerPawnToSuspendPhaser() {
            suspendPhaser.register();
        }

        public void suspendPawn(PawnInterface pawn) {
            phasersById.get(pawn.getID()).arriveAndAwaitAdvance();
            this.suspendPhaser.arriveAndAwaitAdvance();
            this.suspendPhaser.arriveAndAwaitAdvance();
        }
    }

    public static String createLocation(int col, int row) {
        return col + "-" + row;
    }

}
