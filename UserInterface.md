# _User Interface_ #

---


SPARCSIM UI is text based. Simple text commands areused to operate the simulator. Rationales behind the command line interface are as follows:

  * SPARCSIM is designed after TSIM - LEON3 - EVAL simulator by Aeroflex Gaisler to provide exactly same look and feel to those who are already accustomed to using it. Later is a command based one.
  * CLI enables the developer to run a batch script containing simulator commands, thus feeding thousands of commands in no time.
  * Simulation, by its very nature, is a costly operation in terms of CPU clock cycles, as we frequently need to execute a bunch of commands onto the host processor to implement a single command of the target processor.GUI (Graphics User Interface), we feel, is much of a resource hog. Precious resource can be saved by eliminating GUI.
  * The simulator runs on Linux platform and historically, there are more CLI-savvy Linux users than those expecting a nice and colorful UI.
  * Simulation and debugging requires much of expertise. Those users, we expect, will be much more biased towards performance rather a GUI.
  * Lastly, CLI programs always leave a scope to augment it with a GUI and required call-back functions.