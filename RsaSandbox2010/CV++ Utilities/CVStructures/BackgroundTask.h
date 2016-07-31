#pragma once

#define TRY_CANCEL if(_handleCancellation()) return

namespace CVStructures
{
	using namespace System;
	using namespace System::Diagnostics;
	using namespace System::ComponentModel;

	ref class BackgroundTask;

	public delegate void TaskReportProgressHandler(BackgroundTask^ sender);

	public ref class BackgroundTask abstract
	{
		public:
			property bool IsBusy {
				public: bool get() { return _worker->IsBusy; }
			}

			property bool IsCancellationRequested {
				public: bool get() { return _worker->CancellationPending; }
			}

			property bool IsStepCountDefined {
				public: bool get() { return (_steps > 0); }
			}

			property UInt32 StepCount {
				public: UInt32 get() { return _steps; }
				protected: void set(UInt32 value) { _steps = value; }
			}

			property UInt32 CurrentStep {
				public: UInt32 get() { return _currentStep; }
			}

			property float ProgressPercentage {
				public: float get() { return (IsStepCountDefined) ? _currentStep / (float)_steps : -1; }
			}

			property Int64 TimeElapsedMs {
				public: Int64 get() { return _watch->ElapsedMilliseconds; }
			}

			property Int64 ReportProgressInterval {
				public: Int64 get() { return _reportProgressInterval; }
				public: void set(Int64 value) { _reportProgressInterval = (value > 0) ? value : 0; }
			}

			property Int64 TimeToNextReportMs {
				public: Int64 get() { return (_reportProgressInterval - TimeElapsedMs + _lastProgressUpdateTime); }
			}

			property Int64 PredictedTimeLeftMs {
				public: Int64 get()
						{
							if(!IsStepCountDefined) return -1;

							float progressDone = _currentStep / (float)_steps;
							if(progressDone == 0) return -1;

							float progressLeft = 1 - progressDone;
							return (Int64)((progressLeft * (double)TimeElapsedMs) / progressDone);
						}
			}

			event TaskReportProgressHandler^ ReportProgress;

			void requestCancel()
			{
				if(IsBusy) _worker->CancelAsync();
			}

		protected:
			BackgroundTask()
			{
				_worker = gcnew BackgroundWorker();
				_worker->WorkerReportsProgress = true;
				_worker->WorkerSupportsCancellation = true;
				_worker->DoWork += gcnew DoWorkEventHandler(this, &BackgroundTask::Event_DoWork);
				_worker->ProgressChanged += gcnew ProgressChangedEventHandler(this, &BackgroundTask::Event_ProgressChanged);
				_worker->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler(this, &BackgroundTask::Event_RunWorkerCompleted);
				_workerArgs = nullptr;

				_watch = gcnew Stopwatch();
				_currentStep = _steps = 0;
				_reportProgressInterval = 15;
			}

			void _incrementCurrentStep()
			{
				++_currentStep;
			}

			void _increaseCurrentStep(UInt32 incr)
			{
				_currentStep += incr;
			}

			void _resetCurrentStep()
			{
				_currentStep = 0;
			}

			bool _handleStartUp()
			{
				return _handleStartUp(0);
			}

			bool _handleStartUp(UInt32 stepCount)
			{
				if(!IsBusy) {
					_steps = stepCount;
					_worker->RunWorkerAsync();
					return true;
				}
				return false;
			}

			void _handleReportProgress()
			{
				_worker->ReportProgress(0);
			}

			void _forceReportProgress()
			{
				_worker->ReportProgress(1);
			}

			bool _handleCancellation()
			{
				if(_worker->CancellationPending) {
					_workerArgs->Cancel = true;
					return true;
				}
				return false;
			}

			virtual void _run() = 0;
			virtual void _done() = 0;
			virtual void _doneCancelled() = 0;

		private:
			BackgroundWorker^ _worker;
			DoWorkEventArgs^ _workerArgs;
			Stopwatch^ _watch;
			UInt32 _currentStep;
			UInt32 _steps;
			Int64 _reportProgressInterval;
			Int64 _lastProgressUpdateTime;

			void Event_DoWork(Object^ sender, DoWorkEventArgs^ e)
			{
				_lastProgressUpdateTime = 0;
				_workerArgs = e;
				_watch->Restart();
				_run();
			}

			void Event_ProgressChanged(Object^ sender, ProgressChangedEventArgs^ e)
			{
				if(e->ProgressPercentage == 0) {
					Int64 ms = TimeElapsedMs;
					if(ms - _lastProgressUpdateTime > _reportProgressInterval) {
						_lastProgressUpdateTime = ms;
						ReportProgress(this);
					}
				}
				else {
					_lastProgressUpdateTime = TimeElapsedMs;
					ReportProgress(this);
				}
			}

			void Event_RunWorkerCompleted(Object^ sender, RunWorkerCompletedEventArgs^ e)
			{
				_workerArgs = nullptr;
				_currentStep = _steps = 0;
				_watch->Stop();
				_lastProgressUpdateTime = TimeElapsedMs;

				if(e->Cancelled) _doneCancelled();
				else _done();
			}
	};
}