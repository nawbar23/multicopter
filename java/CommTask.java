package com.multicopter.java;



import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by nbar on 2016-08-30.
 * Abstract class for all synchronous communication tasks.
 * Controlled by start and stop methods, dynamically responds for frequency change.
 */
public abstract class CommTask {
    public CommHandler commHandler;

    private Timer timer;

    // frequency of task [Hz]
    private double  frequency;

    private boolean isRunning;

    protected CommTask(CommHandler commHandler) {
     this.commHandler = commHandler;
        this.frequency = 20;
        this.isRunning = false;
    }

    public CommHandler getCommHandler() {
        return commHandler;
    }

    public void start() {
        start(frequency);
    }

    private void start(double freq) {
        System.out.println("Starting task: " + getTaskName() + " with freq: " + String.valueOf(freq) + "Hz");
        timer = new Timer(getTaskName() + "_timer");
        TimerTask timerTask = new TimerTask() {
            @Override
            public void run() {
                task();
            }
        };
        long period = (long)((1.0 / freq) * 1000);
        long delay = period > 200 ? period : 200;
        //Log.e(DEBUG_TAG, "Starting " + getTaskName() + " task with freq: " + String.valueOf(freq) + " Hz, and delay: " + String.valueOf(delay) + " ms");
        timer.scheduleAtFixedRate(timerTask, delay, period);
        isRunning = true;
    }

    public void stop() {
        System.out.println("Stopping task: " + getTaskName());
        timer.cancel();
        isRunning = false;
    }

    public void restart() {
        restart(frequency);
    }

    private void restart(double freq) {
        stop();
        start(freq);
    }

    public void setFrequency(double frequency) {
        this.frequency = frequency;
        if (isRunning){
            stop();
            start();
        }
    }

    public boolean isRunning() {
        return isRunning;
    }

    protected abstract String getTaskName();

    protected abstract void task();


}


