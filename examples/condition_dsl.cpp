// 2020/08/28 - Created by netcan: https://github.com/netcan
// Task DSL demo
// The example creates the following cyclic graph:
//
//       A
//       |
//       v
//       B<---|
//       |    |
//       v    |
//       C----|
//       |
//       v
//       D
//
// - A is a task that initializes a counter to zero
// - B is a task that increments the counter
// - C is a condition task that loops around B until the counter
//   reaches a breaking number
// - D is a task that wraps up the result
#include <taskflow/taskflow.hpp>
#include <taskflow/dsl/task_dsl.hpp>

int main() {
  tf::Executor executor;
  tf::Taskflow taskflow("Conditional Tasking Demo");

  int counter; // owner

  // use context to pass args
  // context must copyable
  struct Context {
    int &rcounter; // use counter(borrow)
  } context{counter};

  def_taskc(A, Context, {
    std::cout << "initializes the counter to zero\n";
    rcounter = 0;
  });
  def_taskc(B, Context, {
    std::cout << "loops to increment the counter\n";
    rcounter++;
  });
  def_taskc(C, Context, {
    std::cout << "counter is " << rcounter << " -> ";
    if (rcounter != 5) {
      std::cout << "loops again (goes to B)\n";
      return 0;
    }
    std::cout << "breaks the loop (goes to D)\n";
    return 1;
  });
  def_taskc(D, Context, {
    std::cout << "done with counter equal to " << rcounter << '\n';
  });

  taskbuild(
    task(A)
      -> task(B)
      -> task(C),
    task(C)
      -> fork(B, D)
  )(taskflow, context);

  // visualizes the taskflow
  taskflow.dump(std::cout);

  // executes the taskflow
  executor.run(taskflow).wait();

  assert(counter == 5);

  return 0;
}
