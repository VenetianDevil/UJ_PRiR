import java.util.ArrayList;

public class RAID__ implements RAIDInterface {
    int matrixState = 0;
    int degradedDiscIdx = -1;
    int diskSize;
    boolean shutdown = false;
    ArrayList<DiskInterface> disks = new ArrayList<>();
    int globalChangedDiskIdx = -1;
    int globalChangedSectorKOldVal = 0;

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
        if(!disks.isEmpty()){
            matrixState = 1;
            initialization();
        }
    }

    private void initialization() {
        System.out.println("initializing");

        for (int sectorK = 0; sectorK < diskSize; sectorK++){
            update_sum_k(sectorK, -1, 0, 0);
        }

//        for (int val : disks.get(disks.size()-1).getSectors() ) {
//            System.out.print(val + "; ");
//        }
//        System.out.println();

        matrixState = 2; //NORMAL
    }

    private void update_sum_k(int k, int changedDiskIdx, int changedSectorKOldVal, int degradedDiscSectorKValue) {
        System.out.println("update sum");
        int sumK = 0;
        for (int diskIdx = 0; diskIdx < disks.size() - 1; diskIdx++){
//            diskIdx == globalChangedDiskIdx - na wypadek jak wywali blad przy odczytywaniu swiezo zmienionej wartosci,
//            a przeciez wlasnie liczymy sume kontrolna wiec nie uda sie tego wyliczyc findem
            if((matrixState == 3 && diskIdx == degradedDiscIdx) || diskIdx == globalChangedDiskIdx){
                sumK += degradedDiscSectorKValue;
            } else {
                sumK += read((diskIdx*diskSize) + k);
            }
        }

        globalChangedDiskIdx = -1;
        globalChangedSectorKOldVal = 0;

//      zapisanie sumy kontrolnej
        System.out.println("update sum write sum " + sumK);
        write(((disks.size() - 1)*diskSize) + k, sumK);
    }

    // func. wywolywana tylko dla dyskow popsutych innych niz ostatni kontrolny
    private int findValueOfKsectorOfDegradedDisk(int k, int changedDiskIdx, int ChangedSectorKOldVal ) {
        int degradedDiscValSectorK = read(((disks.size() - 1)*diskSize) + k); //odczytanie sumy

        for (int diskIdx = 0; diskIdx < disks.size() - 1; diskIdx++){
            if(diskIdx != degradedDiscIdx && diskIdx != changedDiskIdx){
                degradedDiscValSectorK -= read((diskIdx*diskSize) + k);
            } else if (diskIdx == changedDiskIdx){
                degradedDiscValSectorK -= ChangedSectorKOldVal;
            }
        }
        return degradedDiscValSectorK;
    }

    @Override
    public void replaceDisk(DiskInterface disk) {
        System.out.println("replace disk degrIdx: " + degradedDiscIdx);
        disks.set(degradedDiscIdx, disk);
        matrixState = 4; // rebuild
        reconstructNewDisk();
    }

    private void reconstructNewDisk() {
        for (int sectorK = 0; sectorK < diskSize; sectorK++) {
//            jesli to nie jest dysk koncowy
            if(degradedDiscIdx < disks.size() - 1){
                int degradedDiscSectorKVal = findValueOfKsectorOfDegradedDisk(sectorK, -1, 0);
                write((degradedDiscIdx * diskSize) + sectorK, degradedDiscSectorKVal);
            } else { //jesli to dysk koncowy
                update_sum_k(sectorK, -1,0,0);
            }
        }

//        for (int val : disks.get(degradedDiscIdx).getSectors() ) {
//            System.out.print(val + "; ");
//        }
//        System.out.println();

        matrixState = 2;
        degradedDiscIdx = -1;
    }

    @Override
    public void write(int sector, int value) {
        // discSize = 50
        // sector = 100
        synchronized (disks.get(sector/diskSize)){
            int diskIdx = sector / diskSize;
            globalChangedDiskIdx = diskIdx;
    //       przed shutdown() && gdy macierz jest zainicjalizowana
    //       lub jest w czasie inicjalizaji i zapisujemy do ostatniego dysku
            if(!shutdown && (matrixState > 1 || (matrixState == 1 && diskIdx == (disks.size() - 1))) ){

                int discSector = sector % diskSize;
                int degradedDiskSectorKVal = 0;

                if (matrixState == 2) {
                    globalChangedSectorKOldVal = read(sector);
                    System.out.println("reading old val " + globalChangedSectorKOldVal);
                }

                if(matrixState > 2){
        //           jeśli popsuty dysk nie jest ostanim, kontrolnym
                    if (degradedDiscIdx != (disks.size() - 1)) {
                        degradedDiskSectorKVal = findValueOfKsectorOfDegradedDisk(discSector, -1 ,0);
                    }
                }

                if (matrixState == 3 && degradedDiscIdx == diskIdx){
                    degradedDiskSectorKVal = value;
                } else {
                    try {
                        disks.get(diskIdx).write(discSector, value);
                        degradedDiskSectorKVal = value;
                    } catch (DiskInterface.DiskError e) {
                        if (matrixState == 2){ //tylko 1 moze nie dzialac na raz
                            System.out.println("error writing to disk" + diskIdx);
                            matrixState = 3;
                            degradedDiscIdx = diskIdx;
                            degradedDiskSectorKVal = value;
                        } else {
                            write(sector, value);
                        }
                    }
                }

                //jesli ostatni nie jest popsuty && jeśli właśnie nie zapisywaliśmy sumy
                if(degradedDiscIdx != (disks.size() - 1) && diskIdx != (disks.size() - 1)){
                    update_sum_k(discSector, diskIdx, globalChangedSectorKOldVal, degradedDiskSectorKVal);
                }
            }
        }

    }

    @Override
    public int read(int sector) {

        synchronized (disks.get(sector/diskSize)){
            if (!shutdown && matrixState > 0){

                int diskIdx = sector / diskSize;
                int discSector = sector % diskSize;

                int val = 0;

                if (matrixState > 2 && degradedDiscIdx == diskIdx){
                    val = findValueOfKsectorOfDegradedDisk(discSector, -1, 0);
                } else {
                    try {
                        val = disks.get(diskIdx).read(discSector);
                    } catch (DiskInterface.DiskError e){
                        if (matrixState == 2) { //tylko 1 moze nie dzialac na raz
                            System.out.println("error reading from disk" + diskIdx);
                            matrixState = 3;
                            degradedDiscIdx = diskIdx;
                            val = findValueOfKsectorOfDegradedDisk(discSector, globalChangedDiskIdx, globalChangedSectorKOldVal);
                            System.out.println("val, globalChangedDiskIdx, globalChangedSectorKOldVal: " + val + " " + globalChangedDiskIdx + " " + globalChangedSectorKOldVal);
                        } else {
                            // ignorujemy ten error wiec jeszcze jedna próba odczytu, bo nie odczytamy tej wartości jak wyżej,
                            // bo już nam jakiś 1 dysk nie działa
                            val = read(sector);
                        }
                    }
                }

                return val;
            }
            return 0;
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
