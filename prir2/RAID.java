import java.util.ArrayList;
import java.util.Arrays;

class RAID implements RAIDInterface {
    int matrixState = 0;
    int degradedDiscIdx = -1;
    int diskSize;
    boolean shutdown = false;
    ArrayList<DiskInterface> disks = new ArrayList<>();
    ArrayList<Long> threadIdsOnSector;
    ArrayList<Long> countingAllKByThread;
    int[] oldVals;

    @Override
    public RAIDState getState() {
        if (matrixState == 0){ return RAIDState.UNKNOWN; }
        else if (matrixState == 1){ return RAIDState.INITIALIZATION; }
        else if (matrixState == 2){ return RAIDState.NORMAL; }
        else if (matrixState == 3){ return RAIDState.DEGRADED; }
        else { return RAIDState.REBUILD; } //matrixState == 4
    }

    @Override
    public void addDisk(DiskInterface disk) {
        if (disks.isEmpty()){
            diskSize = disk.size();
        }
        disks.add(disk);
    }

    @Override
    public void startRAID() {
        oldVals = new int[size() + diskSize];
        threadIdsOnSector = new ArrayList<>(Arrays.asList(new Long[size() + diskSize]));
        countingAllKByThread = new ArrayList<>(Arrays.asList(new Long[diskSize]));

        matrixState = 1;

        if(!disks.isEmpty()){
            Thread t = new Thread(){
                @Override
                public void run() {
                    initialization();
                }
            };

            t.start();
        }
    }

    private void initialization() {
        // System.out.println("initializing");

        for (int sectorK = 0; sectorK < diskSize; sectorK++){
            update_sum_k(sectorK, 0);
        }

        matrixState = 2; //NORMAL
    }

    private synchronized int[] findSectorKAllVals (int k, int newVal){
        long threadId = Thread.currentThread().getId();

        // System.out.println("przypisalem " +  threadId + " do szukania wartosci k: " + k + " with new val = " + newVal);

        int editedDiskIdx = -1; // nie istnieje
        int editedSectorN = threadIdsOnSector.indexOf(threadId);
        if (editedSectorN != -1){
            editedDiskIdx = editedSectorN / diskSize; // istnieje
        }
        // System.out.println("editedDiskIdx - " + editedDiskIdx);

        int[] disksSectorKVal = new int[disks.size()];
        boolean findDegradedDiskSectorKVal = false;

        for (int diskIdx = 0; diskIdx < disks.size() - 1; diskIdx++){
            int sectorN = (diskIdx*diskSize) + k;
            // System.out.println("\t\tth " +  threadId + " szukam k = " + k + " dla dysku = " + diskIdx);

            if (diskIdx == editedDiskIdx) { //jesli to jest val z dysku wlasnie edytowanego
                disksSectorKVal[diskIdx] = newVal; //moze to tez byc dysk zeputy do ktorego probowalismy zapisac
            } else if (oldVals[sectorN] != -1 && threadIdsOnSector.get(sectorN) != null && editedDiskIdx != diskIdx){
                //bierzemy starą, bo inaczej sie zakleczymy,
                // ale nie z edited, bo tu dajemy nową
                disksSectorKVal[diskIdx] = oldVals[sectorN];
            } else if (diskIdx != degradedDiscIdx && diskIdx != editedDiskIdx ){
//                jeśli dysk nie jest zepsuty i nie byl teraz nadpisany
                int checkReadVal = read(sectorN);
                if (checkReadVal == -1){
                    findDegradedDiskSectorKVal = true; //to znaczy ze teraz nam sie wywalil dysk
                } else {
                    disksSectorKVal[diskIdx] = checkReadVal;
                }
            } else if (diskIdx == degradedDiscIdx && diskIdx != editedDiskIdx ) {
                //jesli to nie jest dysk do ktorego terz zapisywalismy,
                // ale jest zepsuty, to bedziemy szukac jego wartosci
                findDegradedDiskSectorKVal = true;
            }
            // System.out.println("\t\tth " +  threadId + " znalazlem k = " + k + " dla dysku = " + diskIdx + " val = " + disksSectorKVal[diskIdx]);

        }

        //        jeśli brakuje nam wartosci dysku zepsutego, to szukamy go na podstawie juz odczytanych
        if(findDegradedDiskSectorKVal == true){
            int degradedDiskSectorKVal = read(((disks.size() - 1)*diskSize) + k); //odczytanie sumy

            for (int diskIdx = 0; diskIdx < disks.size() - 1; diskIdx++){
                if (diskIdx != degradedDiscIdx){
                    int sectorN = diskIdx*diskSize + k;
                    if (oldVals[sectorN] != -1){ //jesli sektor K ma starą wartość
                        degradedDiskSectorKVal -= oldVals[sectorN];
                    } else { //jesli nie mamy starej wartości
                        degradedDiskSectorKVal -= disksSectorKVal[diskIdx];
                    }
                }
            }
            disksSectorKVal[degradedDiscIdx] = degradedDiskSectorKVal;
        }

        return  disksSectorKVal;
    }

    private void update_sum_k(int k, int newVal) {
        long threadId = Thread.currentThread().getId();
        // System.out.println("update sum by th - " + threadId);
        int[] disksSectorKVal = findSectorKAllVals(k, newVal);
        int sumK = Arrays.stream(disksSectorKVal).sum();

//      zapisanie sumy kontrolnej
        // System.out.println("th " + threadId + " update sum " + k +" write sum " + sumK);
        write(((disks.size() - 1)*diskSize) + k, sumK);

        countingAllKByThread.set(k, null);

        for (int diskIdx = 0; diskIdx < disks.size() - 1; diskIdx++){
            if (diskIdx != degradedDiscIdx){
                int sectorN = diskIdx*diskSize + k;
                if (oldVals[sectorN] != -1){ //jesli sektor K ma starą wartość
                    oldVals[sectorN] = -1; //zerujemy stare wartosci bo juz sa nieaktualne
                }
            }
        }
    }

    // func. wywolywana tylko dla dyskow popsutych innych niz ostatni kontrolny
    private int findValueOfSectorKOfDegradedDisk(int k ) {
        int[] disksSectorKVal = findSectorKAllVals(k, 0);
        countingAllKByThread.set(k, null);

        return disksSectorKVal[degradedDiscIdx];
    }

    @Override
    public void replaceDisk(DiskInterface disk) {
        // System.out.println("replace disk degrIdx: " + degradedDiscIdx);
        disks.set(degradedDiscIdx, disk);
        matrixState = 4; // rebuild
        reconstructNewDisk();
    }

    private void reconstructNewDisk() {
        long threadId = Thread.currentThread().getId();
        // System.out.println("reconstructing by th - " + threadId);

        for (int sectorK = 0; sectorK < diskSize; sectorK++) {
//          jesli to nie jest dysk koncowy

            int[] disksSectorKVal = findSectorKAllVals(sectorK, 0);

            if(degradedDiscIdx < disks.size() - 1){
                // System.out.println("\t\t\treconstructing disk: " + degradedDiscIdx + " sector k = " + sectorK + "with val = " + disksSectorKVal[degradedDiscIdx]);
                write((degradedDiscIdx * diskSize) + sectorK, disksSectorKVal[degradedDiscIdx]);
            } else { //jesli to dysk koncowy
                int sumK = Arrays.stream(disksSectorKVal).sum();
                write((degradedDiscIdx * diskSize) + sectorK, sumK);
            }
        }

        matrixState = 2;
        degradedDiscIdx = -1;
    }

    @Override
    public void write(int sector, int value) {
        int diskIdx = sector / diskSize;
        int sectorK = sector % diskSize;

//        ?? moze sie tak nie bd zakleszczać teraz?
        if(diskIdx != degradedDiscIdx){
            oldVals[sector] = read(sector);
        }

        int newVal = 0; //do przekazania do obliczania sumy kontorlnej, bo po co to odczytywać zaraz po zapisaniu
        synchronized (disks.get(diskIdx)){
//            jesli dysk dziala, odczytaj starą wartość sektora

            if (matrixState < 4 || diskIdx != degradedDiscIdx){
                long threadId = Thread.currentThread().getId();
                // System.out.println("diskIdx - " + diskIdx);

                threadIdsOnSector.set(sector, threadId);
            }

            if (matrixState == 3 && degradedDiscIdx == diskIdx){
                //jeśli zapisujemy do dysku zepsutego,
                // to przekażemy tę wartość do obliczania sumy kontrolnej
                newVal = value;
            } else {
                if (!shutdown && (matrixState > 1 || (matrixState == 1 && diskIdx == (disks.size() - 1)))){
                    try {
                        disks.get(diskIdx).write(sectorK, value);
                        newVal = value; //bo nie uda sie nam odczytać tej wartości, bo blokujemy dostęp do tego dysku
                    } catch (DiskInterface.DiskError e) {
                        if (matrixState == 2){ //tylko 1 moze nie dzialac na raz
                            // System.out.println("error writing to disk" + diskIdx);
                            matrixState = 3;
                            degradedDiscIdx = diskIdx;
                            newVal = value; // popsuty dysk, działamy tak jak w if-e wyzej
                        } else {
                            // jesli to juz ktorys popsuty dysk to próbujemy jeszcze raz bo ignorujemy ten błąd
                            write(sector, value);
                        }
                    }
                } else {
                    return;
                }
            }

        }

            // liczymy sumę kontrolną
            //jesli ostatni nie jest popsuty && jeśli właśnie nie zapisywaliśmy sumy
    //      i jesli to nie jest rekonstrukcja dysku
            if(degradedDiscIdx != (disks.size() - 1) && diskIdx != (disks.size() - 1) && !(matrixState == 4 && diskIdx == degradedDiscIdx)){
                update_sum_k(sectorK, newVal);
            }
            threadIdsOnSector.set(sector, null);
    }

    @Override
    public int read(int sector) {
        int diskIdx = sector / diskSize;
        int sectorK = sector % diskSize;

        synchronized (disks.get(diskIdx)){
            int val = 0;

            if (matrixState > 2 && degradedDiscIdx == diskIdx){
                val = findValueOfSectorKOfDegradedDisk(sectorK);
            } else {
                if (!shutdown && matrixState > 0) {
                    try {
                        val = disks.get(diskIdx).read(sectorK);
                    } catch (DiskInterface.DiskError e){
                        if (matrixState == 2) { //tylko 1 moze nie dzialac na raz
                            // System.out.println("error reading from disk" + diskIdx);
                            matrixState = 3;
                            degradedDiscIdx = diskIdx;

//                            jesli to sie wywaliło przy liczeniu sumy, to wtedy zwracamy cos zeby on wiedzial, zeby oznaczyc te wartosc do szukania
//                            w innym wypadku wywołujemy find (po raz 1) -> val = find...
//                            robimy tak zeby sie w findach nie zapętlić

                            long threadId = Thread.currentThread().getId();
                            int editedSectorN = threadIdsOnSector.indexOf(threadId);
                            if (editedSectorN != -1){
                                // System.out.println("\tread val: " + (-1) );
                                return -1;
                            }

                            val = findValueOfSectorKOfDegradedDisk(sectorK);
                        } else {
                            // ignorujemy ten error wiec jeszcze jedna próba odczytu, bo nie odczytamy tej wartości jak wyżej,
                            // bo już nam jakiś 1 dysk nie działa
                            val = read(sector);
                        }
                    }
                } else {
                    return 0;
                }
            }

            // System.out.println("\tread val: " + val );
            return val;
        }
    }

    @Override
    public int size() {
        return diskSize * (disks.size() - 1);
    }

    @Override
    public void shutdown() {
        shutdown = true;
    }
}
